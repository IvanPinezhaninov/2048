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


#include "gamespec.h"
#include "logger.h"
#include "storageworker.h"
#include "turnspec.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QStringList>

static const char *const DATABASE_TYPE = "QSQLITE";
static const char *const DATABASE_NAME = "database.sqlite3";
#ifdef Q_OS_MACOS
static const char *const DATABASE_FILE_LOCATION = "%1/../Resources/%2";
#else
static const char *const DATABASE_FILE_LOCATION = "%1/%2";
#endif

static const char *const GAME_ID_COLUMN_NAME = "game_id";
static const char *const TURN_ID_COLUMN_NAME = "turn_id";
static const char *const ROWS_COLUMN_NAME = "rows";
static const char *const COLUMNS_COLUMN_NAME = "columns";
static const char *const GAME_STATE_COLUMN_NAME = "game_state";
static const char *const SCORE_COLUMN_NAME = "score";
static const char *const BEST_SCORE_COLUMN_NAME = "best_score";
static const char *const TILE_SPEC_ID_COLUMN_NAME = "tile_spec_id";
static const char *const TILE_SPEC_CELL_COLUMN_NAME = "tile_spec_cell";
static const char *const TILE_SPEC_VALUE_COLUMN_NAME = "tile_spec_value";

static const char *const GAME_STATE_INIT_NAME = "Init";
static const char *const GAME_STATE_PLAY_NAME = "Play";
static const char *const GAME_STATE_WIN_NAME = "Win";
static const char *const GAME_STATE_DEFEAT_NAME = "Defeat";
static const char *const GAME_STATE_CONTINUE_NAME = "Continue";

static const int GAME_STATE_INIT_VALUE = 0;
static const int GAME_STATE_PLAY_VALUE = 1;
static const int GAME_STATE_WIN_VALUE = 2;
static const int GAME_STATE_DEFEAT_VALUE = 3;
static const int GAME_STATE_CONTINUE_VALUE = 4;

static const int MOVE_DIRECTION_NONE_VALUE = 0;
static const int MOVE_DIRECTION_LEFT_VALUE = 1;
static const int MOVE_DIRECTION_RIGHT_VALUE = 2;
static const int MOVE_DIRECTION_UP_VALUE = 3;
static const int MOVE_DIRECTION_DOWN_VALUE = 4;

static const int WRONG_ID = -1;
static const int FIRST_TURN_ID = 0;
static const int WRONG_DATABASE_VERSION = -1;


namespace Game {
namespace Internal {

StorageWorker::StorageWorker() :
    QObject(nullptr),
    m_gameId(WRONG_ID),
    m_turnId(WRONG_ID)
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

    if (!updateGame()) {
        handleCreateGameError();
        return;
    }

    int gameId = WRONG_ID;
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

    m_gameId = gameId;
    m_turnId = FIRST_TURN_ID;
    emit gameCreated();
}


void StorageWorker::saveTurn(const TurnSpec &turn)
{
    if (WRONG_ID == m_gameId) {
        qWarning() << "Failed to save turn in the database. Missing game id";
        handleSaveTurnError(false);
        return;
    }

    if (WRONG_ID == m_turnId) {
        qWarning() << "Failed to save turn in the database. Missing turn id";
        handleSaveTurnError(false);
        return;
    }

    QMutexLocker locker(&m_lock);
    const bool transactional = startTransaction();

    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("INSERT INTO turns (game_id, parent_turn_id, move_direction, "
                                                             "score, best_score) "
                                          "VALUES (?, ?, ?, ?, ?)");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the save turn query:" << qPrintable(sqlQuery.lastError().text());
        handleSaveTurnError();
        return;
    }

    sqlQuery.addBindValue(m_gameId);
    sqlQuery.addBindValue(m_turnId);
    sqlQuery.addBindValue(moveDirectionToInt(turn.moveDirection()));
    sqlQuery.addBindValue(turn.score());
    sqlQuery.addBindValue(turn.bestScore());

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the save turn query:" << qPrintable(sqlQuery.lastError().text());
        handleSaveTurnError();
        return;
    }

    bool ok = false;
    m_turnId = sqlQuery.lastInsertId().toInt(&ok);

    if (!ok) {
        qWarning() << "Failed to get turn id of the saved turn";
        handleSaveTurnError();
        return;
    }

    const bool needToSaveGameState = (GameState::Play != turn.gameState());
    if (needToSaveGameState && !saveGameState(turn.gameState())) {
        handleSaveTurnError();
        return;
    }

    if (!saveTiles(turn.tiles())) {
        handleSaveTurnError();
        return;
    }

    if (transactional && !commitTransaction()) {
        handleSaveTurnError();
        return;
    }

    emit turnSaved();
}


void StorageWorker::restoreGame()
{
    QMutexLocker locker(&m_lock);
    const bool transactional = startTransaction();

    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("SELECT games.game_id, games.rows, games.columns, games.game_state, "
                                                 "turns.turn_id, turns.score, turns.best_score "
                                          "FROM turns "
                                          "LEFT JOIN games ON games.game_id = turns.game_id "
                                          "ORDER BY turns.turn_id DESC LIMIT 1");

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

    bool ok = false;

    m_gameId = sqlQuery.value(QLatin1Literal(GAME_ID_COLUMN_NAME)).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get id of the restored game";
        handleRestoreGameError();
        return;
    }

    const int rows = sqlQuery.value(QLatin1Literal(ROWS_COLUMN_NAME)).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the rows count of the restored game";
        handleRestoreGameError();
        return;
    }

    const int columns = sqlQuery.value(QLatin1Literal(COLUMNS_COLUMN_NAME)).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the columns count of the restored game";
        handleRestoreGameError();
        return;
    }

    m_turnId = sqlQuery.value(QLatin1Literal(TURN_ID_COLUMN_NAME)).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the turn id of the restored game";
        handleRestoreGameError();
        return;
    }

    const int gameStateValue = sqlQuery.value(QLatin1Literal(GAME_STATE_COLUMN_NAME)).toInt(&ok);
    GameState gameState = GameState::Init;
    if (ok) {
        gameState = gameStateFromInt(gameStateValue);
    } else {
        qWarning() << "Failed to get the game state of the restored game";
        handleRestoreGameError();
        return;
    }

    const int score = sqlQuery.value(QLatin1Literal(SCORE_COLUMN_NAME)).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the score of the restored game";
        handleRestoreGameError();
        return;
    }

    const int bestScore = sqlQuery.value(QLatin1Literal(BEST_SCORE_COLUMN_NAME)).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the best score of the restored game";
        handleRestoreGameError();
        return;
    }


    TileSpecs tiles;
    if (!restoreTiles(tiles)) {
        handleRestoreGameError();
        return;
    }

    if (transactional && !commitTransaction()) {
        handleRestoreGameError();
        return;
    }

    QStringList tileDescriptions;
    for (const auto &tile : tiles) {
        tileDescriptions.append(QString(QLatin1Literal("%1-%2")).arg(tile.cell()).arg(tile.value()));
    }

    qDebug().nospace() << "Game restored from the database. Game id: " << m_gameId << ". Turn id: " << m_turnId;
    qDebug().nospace() << "Gameboard size: " << rows << "x" << columns << ". "
                       << "Game state: " << qPrintable(gameStateName(gameState)) << ". "
                       << "Score: " << score << ". Best score: " << bestScore;
    qDebug() << "Tiles:" << qPrintable(tileDescriptions.join(QLatin1Literal("; ")));

    emit gameRestored({ rows, columns, gameState, score, bestScore, tiles });
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


bool StorageWorker::updateGame()
{
    if (WRONG_ID == m_gameId) {
        qWarning() << "Update the game in the database canceled. Missing game id";
        return true;
    }

    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("REPLACE INTO games (game_id, start_time, finish_time, rows, "
                                                              "columns, score, best_score, game_state) "
                                          "SELECT games.game_id, games.start_time, turns.turn_time, games.rows, "
                                                 "games.columns, turns.score, turns.best_score, games.game_state "
                                          "FROM turns "
                                          "LEFT JOIN games ON games.game_id = turns.game_id "
                                          "ORDER BY turns.turn_id DESC LIMIT 1");

    QString error;
    if (executeQuery(query, error)) {
        return true;
    }

    qWarning() << "Failed to execute the update game query:" << qPrintable(sqlQuery.lastError().text());
    return false;
}


bool StorageWorker::createGame(int rows, int columns, int &gameId)
{
    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("INSERT INTO games (rows, columns, finish_time) "
                                          "VALUES (?, ?, ?)");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the create game query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    sqlQuery.addBindValue(rows);
    sqlQuery.addBindValue(columns);
    sqlQuery.addBindValue({});

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the create game query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    bool ok = false;
    gameId = sqlQuery.lastInsertId().toInt(&ok);

    if (!ok) {
        qWarning() << "Failed to get id of the created game";
        gameId = WRONG_ID;
        return false;
    }

    qDebug().nospace() << "New game saved in the database. Game id: " << gameId << ". "
                       << "Gameboard size: " << rows << "x" << columns;
    return true;
}


bool StorageWorker::saveGameState(GameState state)
{
    Q_ASSERT(WRONG_ID != m_gameId);

    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("UPDATE games SET game_state = ? WHERE game_id = ?");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the save game state query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    sqlQuery.addBindValue(gameStateToInt(state));
    sqlQuery.addBindValue(m_gameId);

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the save game state query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    return true;
}


bool StorageWorker::saveTiles(const TileSpecs &tiles)
{
    Q_ASSERT(WRONG_ID != m_turnId);
    Q_ASSERT(!tiles.isEmpty());

    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("INSERT INTO tiles (turn_id, tile_spec_id, tile_spec_cell, tile_spec_value) "
                                          "VALUES (?, ?, ?, ?)");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the save tiles query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    for (const auto &tile : tiles) {
        sqlQuery.addBindValue(m_turnId);
        sqlQuery.addBindValue(tile.id());
        sqlQuery.addBindValue(tile.cell());
        sqlQuery.addBindValue(tile.value());

        if (!sqlQuery.exec()) {
            qWarning() << "Failed to execute the save tiles query:" << qPrintable(sqlQuery.lastError().text());
            return false;
        }
    }

    return true;
}

bool StorageWorker::restoreTiles(TileSpecs &tiles)
{
    Q_ASSERT(tiles.isEmpty());
    Q_ASSERT(WRONG_ID != m_turnId);

    QSqlQuery sqlQuery(m_db);

    const QString &query = QLatin1Literal("SELECT tile_spec_id, tile_spec_cell, tile_spec_value "
                                          "FROM tiles "
                                          "WHERE turn_id = ? "
                                          "ORDER BY tile_spec_cell");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the restore tiles query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    sqlQuery.addBindValue(m_turnId);

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the restore tiles query:" << qPrintable(sqlQuery.lastError().text());
        return false;
    }

    if (!sqlQuery.first()) {
        qWarning() << "Failed to restore tiles. Tiles not found";
        return false;
    }

    bool ok = false;

    do {
        const int id = sqlQuery.value(QLatin1Literal(TILE_SPEC_ID_COLUMN_NAME)).toInt(&ok);
        if (!ok) {
            qWarning() << "Failed to get the tile id of the restored tile";
            return false;
        }

        const int cell = sqlQuery.value(QLatin1Literal(TILE_SPEC_CELL_COLUMN_NAME)).toInt(&ok);
        if (!ok) {
            qWarning() << "Failed to get the cell index of the restored tile";
            return false;
        }

        const int value = sqlQuery.value(QLatin1Literal(TILE_SPEC_VALUE_COLUMN_NAME)).toInt(&ok);
        if (!ok) {
            qWarning() << "Failed to get the tile value of the restored tile";
            return false;
        }

        tiles.append({ id, cell, value });
    } while (sqlQuery.next());

    return true;
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


GameState StorageWorker::gameStateFromInt(int value) const
{
    switch (value) {
    case GAME_STATE_PLAY_VALUE:
        return GameState::Play;
    case GAME_STATE_WIN_VALUE:
        return GameState::Win;
    case GAME_STATE_DEFEAT_VALUE:
        return GameState::Defeat;
    case GAME_STATE_CONTINUE_VALUE:
        return GameState::Continue;
    default:
        return GameState::Init;
    }
}


QString StorageWorker::gameStateName(GameState state) const
{
    switch (state) {
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


MoveDirection StorageWorker::moveDirectionFromInt(int value) const
{
    switch (value) {
    case MOVE_DIRECTION_LEFT_VALUE:
        return MoveDirection::Left;
    case MOVE_DIRECTION_RIGHT_VALUE:
        return MoveDirection::Right;
    case MOVE_DIRECTION_UP_VALUE:
        return MoveDirection::Up;
    case MOVE_DIRECTION_DOWN_VALUE:
        return MoveDirection::Down;
    default:
        return MoveDirection::None;
    }
}


void StorageWorker::handleCreateGameError(bool rollback)
{
    if (rollback) {
        rollbackTransaction();
    }

    m_gameId = WRONG_ID;
    emit createGameError();
}


void StorageWorker::handleSaveTurnError(bool rollback)
{
    if (rollback) {
        rollbackTransaction();
    }

    m_turnId = WRONG_ID;
    emit saveTurnError();
}


void StorageWorker::handleRestoreGameError(bool rollback)
{
    if (rollback) {
        rollbackTransaction();
    }

    m_gameId = WRONG_ID;
    m_turnId = WRONG_ID;
    emit restoreGameError();
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
