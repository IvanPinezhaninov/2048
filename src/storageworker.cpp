/***************************************************************************
**
** Copyright (C) 2018 Ivan Pinezhaninov <ivan.pinezhaninov@gmail.com>
**
** This file is part of the 2048 Game.
**
** The 2048 Game is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** The 2048 Game is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with the 2048 Game.  If not, see <http://www.gnu.org/licenses/>.
**
***************************************************************************/


#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QStringList>
#include <QVariant>
#include <QVariantList>
#include <QVariantMap>

#include "logger.h"
#include "storageworker.h"
#include "storageconstants.h"


static const char *const DATABASE_TYPE = "QSQLITE";
static const char *const DATABASE_NAME = "database.sqlite3";
#ifdef Q_OS_MACOS
static const char *const DATABASE_FILE_LOCATION = "%1/../Resources/%2";
#else
static const char *const DATABASE_FILE_LOCATION = "%1/%2";
#endif

static const char *const BEST_SCORE_COLUMN_NAME = "best_score";
static const char *const COLUMNS_COLUMN_NAME = "columns";
static const char *const GAME_ID_COLUMN_NAME = "game_id";
static const char *const GAME_STATE_COLUMN_NAME = "game_state";
static const char *const PARENT_TURN_ID_COLUMN_NAME = "parent_turn_id";
static const char *const ROWS_COLUMN_NAME = "rows";
static const char *const MOVE_DIRECTION_COLUMN_NAME = "move_direction";
static const char *const SCORE_COLUMN_NAME = "score";
static const char *const TILE_CELL_COLUMN_NAME = "cell_index";
static const char *const TILE_ID_COLUMN_NAME = "tile_id";
static const char *const TILE_VALUE_COLUMN_NAME = "tile_value";
static const char *const TILE_STATE_COLUMN_NAME = "tile_state";
static const char *const TURN_ID_COLUMN_NAME = "turn_id";

static const char *const GAME_STATE_INIT_NAME = "I";
static const char *const GAME_STATE_PLAY_NAME = "P";
static const char *const GAME_STATE_WIN_NAME = "W";
static const char *const GAME_STATE_DEFEAT_NAME = "D";
static const char *const GAME_STATE_CONTINUE_NAME = "C";

static const int GAME_STATE_INIT_VALUE = 0;
static const int GAME_STATE_PLAY_VALUE = 1;
static const int GAME_STATE_WIN_VALUE = 2;
static const int GAME_STATE_DEFEAT_VALUE = 3;
static const int GAME_STATE_CONTINUE_VALUE = 4;

static const char *const MOVE_DIRECTION_NONE_NAME = "N";
static const char *const MOVE_DIRECTION_LEFT_NAME = "L";
static const char *const MOVE_DIRECTION_RIGHT_NAME = "R";
static const char *const MOVE_DIRECTION_UP_NAME = "U";
static const char *const MOVE_DIRECTION_DOWN_NAME = "D";

static const int MOVE_DIRECTION_NONE_VALUE = 0;
static const int MOVE_DIRECTION_LEFT_VALUE = 1;
static const int MOVE_DIRECTION_RIGHT_VALUE = 2;
static const int MOVE_DIRECTION_UP_VALUE = 3;
static const int MOVE_DIRECTION_DOWN_VALUE = 4;

static const int WRONG_ID = -1;
static const int WRONG_DATABASE_VERSION = -1;


namespace Game {
namespace Internal {

StorageWorker::StorageWorker() :
    QObject(nullptr)
{
}


void StorageWorker::openDatabase()
{
    Q_ASSERT(QSqlDatabase::isDriverAvailable(QLatin1Literal(DATABASE_TYPE)));
    m_db = QSqlDatabase::addDatabase(QLatin1Literal(DATABASE_TYPE));

    if (!m_db.isValid()) {
        qWarning() << "Failed to add database:" << qPrintable(m_db.lastError().text());
        emit storageError();
        return;
    }

#if defined(Q_OS_OSX)
    const QString &databaseName = QString(QLatin1Literal(DATABASE_FILE_LOCATION))
            .arg(QCoreApplication::applicationDirPath(), QLatin1Literal(DATABASE_NAME));
#else
#if (QT_VERSION < QT_VERSION_CHECK(5, 4, 0))
    const QString &databaseDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    const QString &databaseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif
    const QString &databaseName = QString(QLatin1Literal(DATABASE_FILE_LOCATION))
                                  .arg(databaseDir, QLatin1Literal(DATABASE_NAME));

    const QDir dir(databaseDir);
    if (!dir.exists()) {
        if (!dir.mkpath(databaseDir)) {
            qWarning() << "Failed to create database directory:" << qPrintable(databaseDir);
            emit storageError();
            return;
        }
    }
#endif

    m_db.setDatabaseName(databaseName);

    if (!m_db.open()) {
        qWarning() << "Failed to open database:" << databaseName << qPrintable(m_db.lastError().text());
        emit storageError();
        return;
    }

    const int version = databaseVersion();

    if (WRONG_DATABASE_VERSION == version) {
        emit storageError();
        return;
    }

    bool ready = false;

    switch (version) {
    case 1:
        ready = true;
        break;
    default:
        ready = createDatabase();
        break;
    }

    qDebug() << "Database opened:" << qPrintable(databaseName);
    qDebug() << "Database version:" << version;

    if (ready) {
        emit storageReady();
    } else {
        emit storageError();
    }
}


void StorageWorker::closeDatabase()
{
    if (m_db.isOpen()) {
        vacuum();
        m_db.close();
        qDebug() << "Database closed";
    }
}


void StorageWorker::createGame(int rows, int columns)
{
    QMutexLocker locker(&m_lock);
    const bool transactional = startTransaction();

    if (!finishGame()) {
        handleCreateGameError();
        return;
    }

    QVariant gameId;
    if (!createGame(rows, columns, gameId)) {
        handleCreateGameError();
        return;
    }

    removeTiles();
    removeTurns();

    if (transactional && !commitTransaction()) {
        handleCreateGameError();
        return;
    }

    qDebug().nospace() << "C | " << gameId.toInt() << " | " << rows << "x" << columns;

    QVariantMap game;
    game.insert(QLatin1Literal(GAME_ID_KEY), gameId);
    game.insert(QLatin1Literal(ROWS_KEY), rows);
    game.insert(QLatin1Literal(COLUMNS_KEY), columns);

    emit gameCreated(game);
}


void StorageWorker::saveTurn(const QVariantMap &turn)
{
    QMutexLocker locker(&m_lock);

    Q_ASSERT_X(turn.contains(QLatin1Literal(GAME_ID_KEY)), "saveTurn", "Missing game id");
    Q_ASSERT_X(turn.contains(QLatin1Literal(TURN_ID_KEY)), "saveTurn", "Missing turn id");
    Q_ASSERT_X(turn.contains(QLatin1Literal(PARENT_TURN_ID_KEY)), "saveTurn", "Missing parent turn id");
    Q_ASSERT_X(turn.contains(QLatin1Literal(MOVE_DIRECTION_KEY)), "saveTurn", "Missing move direction");
    Q_ASSERT_X(turn.contains(QLatin1Literal(GAME_STATE_KEY)), "saveTurn", "Missing game state");
    Q_ASSERT_X(turn.contains(QLatin1Literal(SCORE_KEY)), "saveTurn", "Missing score");
    Q_ASSERT_X(turn.contains(QLatin1Literal(BEST_SCORE_KEY)), "saveTurn", "Missing best score");
    Q_ASSERT_X(turn.contains(QLatin1Literal(TILES_KEY)), "saveTurn", "Missing tiles");

    const QVariant &turnId = turn.value(QLatin1Literal(TURN_ID_KEY));
    const QVariant &parentTurnId = turn.value(QLatin1Literal(PARENT_TURN_ID_KEY));
    const QVariant &moveDirection = turn.value(QLatin1Literal(MOVE_DIRECTION_KEY));
    const QVariant &score = turn.value(QLatin1Literal(SCORE_KEY));
    const QVariant &bestScore = turn.value(QLatin1Literal(BEST_SCORE_KEY));

    const bool transactional = startTransaction();

    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("INSERT INTO turns (turn_id, parent_turn_id, move_direction, score, best_score) "
                                          "VALUES (?, ?, ?, ?, ?)");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the save turn query:" << qPrintable(sqlQuery.lastError().text());
        handleSaveTurnError();
        return;
    }

    sqlQuery.addBindValue(turnId);
    sqlQuery.addBindValue(parentTurnId);
    sqlQuery.addBindValue(moveDirectionToInt(moveDirection));
    sqlQuery.addBindValue(score);
    sqlQuery.addBindValue(bestScore);

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the save turn query:" << qPrintable(sqlQuery.lastError().text());
        handleSaveTurnError();
        return;
    }

    Q_ASSERT(turn.value(QLatin1Literal(GAME_STATE_KEY)).canConvert<GameState>());

    const QVariant &gameId = turn.value(QLatin1Literal(GAME_ID_KEY));
    const GameState gameState = turn.value(QLatin1Literal(GAME_STATE_KEY)).value<GameState>();

    const bool needToSaveGameState = (GameState::Play != gameState);
    if (needToSaveGameState && !saveGameState(gameId, gameState)) {
        handleSaveTurnError();
        return;
    }

    const QVariantList &tiles = turn.value(QLatin1Literal(TILES_KEY)).toList();
    if (!saveTiles(turnId, tiles)) {
        handleSaveTurnError();
        return;
    }

    if (transactional && !commitTransaction()) {
        handleSaveTurnError();
        return;
    }

    qDebug().nospace() << "S | " << turnId.toInt() << " | " << parentTurnId.toInt() << " | "
                       << qPrintable(gameStateName(gameState)) << " | "
                       << qPrintable(moveDirectionName(moveDirection)) << " | "
                       << score.toInt() << " | " << bestScore.toInt() << " | "
                       << qPrintable(tilesToString(tiles));

    emit turnSaved();
}


void StorageWorker::undoTurn(int turnId)
{
    Q_UNUSED(turnId)
    qDebug() << "Coming soon!";
    emit undoTurnError();
}


void StorageWorker::restoreGame()
{
    QMutexLocker locker(&m_lock);
    const bool transactional = startTransaction();

    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("SELECT games.game_id, games.rows, games.columns, games.game_state, "
                                                 "turns.turn_id, turns.parent_turn_id, turns.score, turns.best_score "
                                          "FROM games, turns "
                                          "ORDER BY games.game_id DESC, turns.turn_id DESC LIMIT 1");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the restore game query:" << qPrintable(sqlQuery.lastError().text());
        handleRestoreGameError();
        return;
    }

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the restore game query:" << qPrintable(sqlQuery.lastError().text());
        handleRestoreGameError();
        return;
    }

    if (!sqlQuery.first()) {
        qWarning() << "Failed to restore game. Game not found";
        handleRestoreGameError();
        return;
    }

    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(GAME_ID_COLUMN_NAME)), "Restore game", "Game id column not found");
    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(ROWS_COLUMN_NAME)), "Restore game", "Rows column not found");
    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(COLUMNS_COLUMN_NAME)), "Restore game", "Columns column not found");
    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(TURN_ID_COLUMN_NAME)), "Restore game", "Turn id column not found");
    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(PARENT_TURN_ID_COLUMN_NAME)), "Restore game", "Parent turn id column not found");
    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(GAME_STATE_COLUMN_NAME)), "Restore game", "Game state column not found");
    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(SCORE_COLUMN_NAME)), "Restore game", "Score column not found");
    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(BEST_SCORE_COLUMN_NAME)), "Restore game", "Best score column not found");

    const QVariant &gameId = sqlQuery.value(QLatin1Literal(GAME_ID_COLUMN_NAME));
    const QVariant &rows = sqlQuery.value(QLatin1Literal(ROWS_COLUMN_NAME));
    const QVariant &columns = sqlQuery.value(QLatin1Literal(COLUMNS_COLUMN_NAME));
    const QVariant &turnId = sqlQuery.value(QLatin1Literal(TURN_ID_COLUMN_NAME));
    const QVariant &parentTurnId = sqlQuery.value(QLatin1Literal(PARENT_TURN_ID_COLUMN_NAME));
    const QVariant &gameState = sqlQuery.value(QLatin1Literal(GAME_STATE_COLUMN_NAME));
    const QVariant &score = sqlQuery.value(QLatin1Literal(SCORE_COLUMN_NAME));
    const QVariant &bestScore = sqlQuery.value(QLatin1Literal(BEST_SCORE_COLUMN_NAME));

    bool ok = false;
    const QVariant &maxTurnId = getMaxTurnId(ok);
    if (!ok) {
        qWarning() << "Failed to get the max turn id of the restored game";
        handleRestoreGameError();
        return;
    }

    const QVariantList &tiles = restoreTiles(turnId, ok);
    if (!ok) {
        handleRestoreGameError();
        return;
    }

    if (transactional && !commitTransaction()) {
        handleRestoreGameError();
        return;
    }

    qDebug().nospace() << "R | " << gameId.toInt()
                       << " | " << rows.toInt() << "x" << columns.toInt() << " | "
                       << turnId.toInt() << " | " << parentTurnId.toInt() << " | " << maxTurnId.toInt() << " | "
                       << qPrintable(gameStateName(gameState)) << " | "
                       << score.toInt() << " | " << bestScore.toInt() << " | "
                       << qPrintable(tilesToString(tiles));

    QVariantMap game;
    game.insert(QLatin1Literal(GAME_ID_KEY), gameId);
    game.insert(QLatin1Literal(ROWS_KEY), rows);
    game.insert(QLatin1Literal(COLUMNS_KEY), columns);
    game.insert(QLatin1Literal(TURN_ID_KEY), turnId);
    game.insert(QLatin1Literal(PARENT_TURN_ID_KEY), parentTurnId);
    game.insert(QLatin1Literal(MAX_TURN_ID_KEY), maxTurnId);
    game.insert(QLatin1Literal(GAME_STATE_KEY), gameState);
    game.insert(QLatin1Literal(SCORE_KEY), score);
    game.insert(QLatin1Literal(BEST_SCORE_KEY), bestScore);
    game.insert(QLatin1Literal(TILES_KEY), tiles);

    emit gameRestored(game);
}


int StorageWorker::databaseVersion()
{
    QSqlQuery sqlQuery(m_db);

    if (!sqlQuery.exec(QLatin1Literal("PRAGMA user_version"))) {
        qWarning() << "Failed to get database version:" << qPrintable(sqlQuery.lastError().text());
        return WRONG_DATABASE_VERSION;
    }

    if (!sqlQuery.first()) {
        qWarning() << "Failed to get database version: database version not found";
        return WRONG_DATABASE_VERSION;
    }

    bool ok = false;
    const int version = sqlQuery.value(0).toInt(&ok);

    if (!ok) {
        qWarning() << "Failed to get database version: incorrect database version";
        return WRONG_DATABASE_VERSION;
    }

    return version;
}


bool StorageWorker::createDatabase()
{
    Q_ASSERT(m_db.isValid());

    if (!executeFileQueries(QLatin1Literal("://sql/configure.sql"))) {
        return false;
    }

    const bool transactional = startTransaction();

    if (!executeFileQueries(QLatin1Literal("://sql/tables.sql"))) {
        rollbackTransaction();
        return false;
    }

    if (transactional && !commitTransaction()) {
        return false;
    }

    return true;
}


bool StorageWorker::executeQuery(const QString &query, QString &error)
{
    QSqlQuery sqlQuery(m_db);

    if (sqlQuery.exec(query)) {
        return true;
    }

    error = sqlQuery.lastError().text();
    return false;
}


bool StorageWorker::executeFileQueries(const QString &fileName)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << qPrintable(fileName);
        return false;
    }

    const QStringList &queries = QString(file.readAll()).simplified().split(QLatin1Char(';'), QString::SkipEmptyParts);

    QSqlQuery sqlQuery(m_db);

    for (const auto &query : queries) {
        if (!sqlQuery.exec(query)) {
            qWarning() << "Failed to execute query:" << sqlQuery.lastError().text();
            return false;
        }
    }

    return true;
}


bool StorageWorker::finishGame()
{
    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("REPLACE INTO games (game_id, start_time, finish_time, rows, "
                                                              "columns, score, best_score, game_state) "
                                          "SELECT games.game_id, games.start_time, turns.turn_time, games.rows, "
                                                 "games.columns, turns.score, turns.best_score, games.game_state "
                                          "FROM games, turns "
                                          "ORDER BY games.game_id DESC, turns.turn_id DESC LIMIT 1");

    QString error;
    if (executeQuery(query, error)) {
        return true;
    }

    qWarning() << "Failed to execute the update game query:" << qPrintable(sqlQuery.lastError().text());
    return false;
}


bool StorageWorker::createGame(int rows, int columns, QVariant &gameId)
{
    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("INSERT INTO games (rows, columns) VALUES (?, ?)");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the create game query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    sqlQuery.addBindValue(rows);
    sqlQuery.addBindValue(columns);

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the create game query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    gameId = sqlQuery.lastInsertId();
    return gameId.isValid();
}


bool StorageWorker::saveGameState(const QVariant &gameId, GameState state)
{
    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("UPDATE games SET game_state = ? WHERE game_id = ?");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the save game state query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    sqlQuery.addBindValue(gameStateToInt(state));
    sqlQuery.addBindValue(gameId);

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the save game state query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    return true;
}


bool StorageWorker::saveTiles(const QVariant &turnId, const QVariantList &tiles)
{
    Q_ASSERT_X(!tiles.isEmpty(), "Save tiles", "There is no tiles for save");

    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("INSERT INTO tiles (turn_id, tile_id, tile_value, cell_index) "
                                          "VALUES (?, ?, ?, ?)");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the save tiles query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    for (const QVariant &var : tiles) {
        const QVariantMap &tile = var.toMap();
        sqlQuery.addBindValue(turnId);
        sqlQuery.addBindValue(tile.value(QLatin1Literal(TILE_ID_KEY)));
        sqlQuery.addBindValue(tile.value(QLatin1Literal(TILE_VALUE_KEY)));
        sqlQuery.addBindValue(tile.value(QLatin1Literal(TILE_CELL_KEY)));

        if (!sqlQuery.exec()) {
            qWarning() << "Failed to execute the save tiles query:" << qPrintable(sqlQuery.lastError().text());
            return false;
        }
    }

    return true;
}


QVariantList StorageWorker::restoreTiles(const QVariant &turnId, bool &ok)
{
    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("SELECT tile_id, cell_index, tile_value "
                                          "FROM tiles WHERE turn_id = ? ORDER BY cell_index");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the restore tiles query:" << qPrintable(sqlQuery.lastError().text());
        ok = false;
        return QVariantList();
    }

    sqlQuery.addBindValue(turnId);

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the restore tiles query:" << qPrintable(sqlQuery.lastError().text());
        ok = false;
        return QVariantList();
    }

    if (!sqlQuery.first()) {
        qWarning() << "Failed to restore tiles. Tiles not found";
        ok = false;
        return QVariantList();
    }

    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(TILE_ID_COLUMN_NAME)), "Restore tiles", "Tile id column not found");
    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(TILE_CELL_COLUMN_NAME)), "Restore tiles", "Tile cell column not found");
    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(TILE_VALUE_COLUMN_NAME)), "Restore tiles", "Tile value column not found");

    QVariantList tiles;

    do {
        QVariantMap tileMap;
        tileMap.insert(QLatin1Literal(TILE_ID_KEY), sqlQuery.value(QLatin1Literal(TILE_ID_COLUMN_NAME)));
        tileMap.insert(QLatin1Literal(TILE_CELL_KEY), sqlQuery.value(QLatin1Literal(TILE_CELL_COLUMN_NAME)));
        tileMap.insert(QLatin1Literal(TILE_VALUE_KEY), sqlQuery.value(QLatin1Literal(TILE_VALUE_COLUMN_NAME)));
        tiles.append(tileMap);
    } while (sqlQuery.next());

    ok = true;

    return tiles;
}


QVariant StorageWorker::getMaxTurnId(bool &ok) const
{
    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("SELECT MAX(turn_id) AS turn_id FROM turns");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the max turn id query:" << qPrintable(sqlQuery.lastError().text());
        ok = false;
        return QVariant();
    }

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the max turn id query:" << qPrintable(sqlQuery.lastError().text());
        ok = false;
        return QVariant();
    }

    if (!sqlQuery.first()) {
        qWarning() << "Failed to get max turn id. The max turn id not found";
        ok = false;
        return QVariant();
    }

    Q_ASSERT_X(sqlQuery.record().contains(QLatin1Literal(TURN_ID_COLUMN_NAME)), "Get max turn id", "Turn id column not found");

    const QVariant &result = sqlQuery.value(QLatin1Literal(TURN_ID_COLUMN_NAME));
    ok = result.isValid();

    return result;
}


void StorageWorker::removeTurns()
{
    QString error;
    if (!executeQuery(QLatin1Literal("DELETE FROM turns"), error)) {
        qWarning() << "Failed to execute the remove turns query" << qPrintable(error);
    }
}


void StorageWorker::removeTiles()
{
    QString error;
    if (!executeQuery(QLatin1Literal("DELETE FROM tiles"), error)) {
        qWarning() << "Failed to execute the remove tiles query:" << qPrintable(error);
    }
}


void StorageWorker::vacuum()
{
    QString error;
    if (!executeQuery(QLatin1Literal("VACUUM"), error)) {
        qWarning() << "Failed to execute the vacuum query:" << qPrintable(error);
    }
}


int StorageWorker::gameStateToInt(GameState gameState) const
{
    switch (gameState) {
    case GameState::Init:
        return GAME_STATE_INIT_VALUE;
    case GameState::Play:
        return GAME_STATE_PLAY_VALUE;
    case GameState::Win:
        return GAME_STATE_WIN_VALUE;
    case GameState::Defeat:
        return GAME_STATE_DEFEAT_VALUE;
    case GameState::Continue:
        return GAME_STATE_CONTINUE_VALUE;
    }
}


QString StorageWorker::gameStateName(const QVariant &gameState) const
{
    Q_ASSERT(gameState.canConvert<GameState>());
    return gameStateName(gameState.value<GameState>());
}


QString StorageWorker::gameStateName(GameState gameState) const
{
    switch (gameState) {
    case GameState::Init:
        return QLatin1Literal(GAME_STATE_INIT_NAME);
    case GameState::Play:
        return QLatin1Literal(GAME_STATE_PLAY_NAME);
    case GameState::Win:
        return QLatin1Literal(GAME_STATE_WIN_NAME);
    case GameState::Defeat:
        return QLatin1Literal(GAME_STATE_DEFEAT_NAME);
    case GameState::Continue:
        return QLatin1Literal(GAME_STATE_CONTINUE_NAME);
    }
}


int StorageWorker::moveDirectionToInt(const QVariant &moveDirection) const
{
    Q_ASSERT(moveDirection.canConvert<MoveDirection>());
    return moveDirectionToInt(moveDirection.value<MoveDirection>());
}


int StorageWorker::moveDirectionToInt(MoveDirection moveDirection) const
{
    switch (moveDirection) {
    case MoveDirection::None:
        return MOVE_DIRECTION_NONE_VALUE;
    case MoveDirection::Left:
        return MOVE_DIRECTION_LEFT_VALUE;
    case MoveDirection::Right:
        return MOVE_DIRECTION_RIGHT_VALUE;
    case MoveDirection::Up:
        return MOVE_DIRECTION_UP_VALUE;
    case MoveDirection::Down:
        return MOVE_DIRECTION_DOWN_VALUE;
    }
}


QString StorageWorker::moveDirectionName(const QVariant &moveDirection) const
{
    Q_ASSERT(moveDirection.canConvert<MoveDirection>());
    return moveDirectionName(moveDirection.value<MoveDirection>());
}


QString StorageWorker::moveDirectionName(MoveDirection moveDirection) const
{
    switch (moveDirection) {
    case MoveDirection::None:
        return QLatin1Literal(MOVE_DIRECTION_NONE_NAME);
    case MoveDirection::Left:
        return QLatin1Literal(MOVE_DIRECTION_LEFT_NAME);
    case MoveDirection::Right:
        return QLatin1Literal(MOVE_DIRECTION_RIGHT_NAME);
    case MoveDirection::Up:
        return QLatin1Literal(MOVE_DIRECTION_UP_NAME);
    case MoveDirection::Down:
        return QLatin1Literal(MOVE_DIRECTION_DOWN_NAME);
    }
}


QString StorageWorker::tilesToString(const QVariantList &tiles) const
{
    QStringList stringList;

    for (const QVariant &var : tiles) {
        const QVariantMap &tile = var.toMap();
        const int cell = tile.value(QLatin1Literal(TILE_CELL_KEY)).toInt();
        const int value = tile.value(QLatin1Literal(TILE_VALUE_KEY)).toInt();
        stringList.append(QString(QLatin1Literal("%1 [%2]")).arg(cell).arg(value));
    }

    std::sort(stringList.begin(), stringList.end());

    return stringList.join(QLatin1Literal("; "));
}


void StorageWorker::handleCreateGameError(bool rollback)
{
    if (rollback) {
        rollbackTransaction();
    }

    emit createGameError();
}


void StorageWorker::handleRestoreGameError(bool rollback)
{
    if (rollback) {
        rollbackTransaction();
    }

    emit restoreGameError();
}


void StorageWorker::handleSaveTurnError(bool rollback)
{
    if (rollback) {
        rollbackTransaction();
    }

    emit saveTurnError();
}


void StorageWorker::handleUndoTurnError(bool rollback)
{
    if (rollback) {
        rollbackTransaction();
    }

    emit undoTurnError();
}


bool StorageWorker::startTransaction()
{
    const bool success = m_db.transaction();

    if (!success) {
        qWarning() << "Start transaction error:" << qPrintable(m_db.lastError().text());
    }

    return success;
}


bool StorageWorker::commitTransaction()
{
    const bool success = m_db.commit();

    if (!success) {
        qWarning() << "Commit transaction error:" << qPrintable(m_db.lastError().text());
    }

    return success;
}


bool StorageWorker::rollbackTransaction()
{
    const bool success = m_db.rollback();

    if (!success) {
        qWarning() << "Rollback transaction error:" << qPrintable(m_db.lastError().text());
    }

    return success;
}

} // namespace Internal
} // namespace Game
