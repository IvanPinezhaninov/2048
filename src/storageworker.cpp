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


#include "storage.h"
#include "storageworker.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QStandardPaths>

static const char *const DATABASE_TYPE = "QSQLITE";
static const char *const DATABASE_NAME = "database.sqlite3";
#ifdef Q_OS_MACOS
static const char *const DATABASE_FILE_LOCATION = "%1/../Resources/%2";
#else
static const char *const DATABASE_FILE_LOCATION = "%1/%2";
#endif

static const int MOVE_DIRECTION_NONE = 0;
static const int MOVE_DIRECTION_LEFT = 1;
static const int MOVE_DIRECTION_RIGHT = 2;
static const int MOVE_DIRECTION_UP = 3;
static const int MOVE_DIRECTION_DOWN = 4;

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
        qWarning() << "Failed to add database:" << qPrintable(m_db.lastError().driverText());
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
        qWarning() << "Failed to open database:" << databaseName << qPrintable(m_db.lastError().driverText());
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

    qDebug() << "Database version:" << version;

    if (ready) {
        emit storageReady();
    } else {
        emit storageError();
    }
}


void StorageWorker::closeDatabase()
{
    m_db.close();
}


void StorageWorker::createGame(int rows, int columns)
{
    QMutexLocker locker(&m_lock);
    QSqlQuery sqlQuery(m_db);
    m_db.transaction();

    const QString &query = QLatin1Literal("INSERT INTO games (rows, columns) "
                                          "VALUES (?, ?)");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the create game query:" << qPrintable(sqlQuery.lastError().driverText());
        handleCreateGameError();
        return;
    }

    sqlQuery.addBindValue(rows);
    sqlQuery.addBindValue(columns);

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the create game query:" << qPrintable(sqlQuery.lastError().driverText());
        handleCreateGameError();
        return;
    }

    bool ok = false;
    m_gameId = sqlQuery.lastInsertId().toInt(&ok);

    if (!ok) {
        qWarning() << "Failed to get id of the created game";
        handleCreateGameError();
        return;
    }

    m_db.commit();
    m_turnId = FIRST_TURN_ID;
    emit gameCreated();
}


void StorageWorker::saveTurn(MoveDirection direction, const TileSpecs &tiles, int score, int bestScore)
{
    QMutexLocker locker(&m_lock);

    if (WRONG_ID == m_gameId || WRONG_ID == m_turnId) {
        qWarning() << "Failed to get either a game id or a turn id for the game save";
        handleSaveGameError();
        return;
    }

    QSqlQuery sqlQuery(m_db);
    m_db.transaction();

    const QString &query = QLatin1Literal("INSERT INTO turns (game_id, parent_turn_id, move_direction, score, best_score) "
                                          "VALUES (?, ?, ?, ?, ?)");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the save game query:" << qPrintable(sqlQuery.lastError().driverText());
        handleSaveGameError();
        return;
    }

    sqlQuery.addBindValue(m_gameId);
    sqlQuery.addBindValue(m_turnId);
    sqlQuery.addBindValue(moveDirection(direction));
    sqlQuery.addBindValue(score);
    sqlQuery.addBindValue(bestScore);

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the save game query:" << qPrintable(sqlQuery.lastError().driverText());
        handleSaveGameError();
        return;
    }

    bool ok = false;
    m_turnId = sqlQuery.lastInsertId().toInt(&ok);

    if (!ok) {
        qWarning() << "Failed to get turn id of the saved game";
        handleSaveGameError();
        return;
    }

    if (!saveTiles(tiles)) {
        handleSaveGameError();
        return;
    }

    m_db.commit();
    emit gameSaved();
}


void StorageWorker::restoreGame()
{
    QMutexLocker locker(&m_lock);

    QSqlQuery sqlQuery(m_db);
    m_db.transaction();

    const QString &query = QLatin1Literal("SELECT games.game_id, games.rows, games.columns, "
                                                 "turns.turn_id, turns.score, turns.best_score "
                                          "FROM turns "
                                          "LEFT JOIN games ON games.game_id = turns.game_id "
                                          "ORDER BY turns.turn_id DESC LIMIT 1");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the restore game query:" << qPrintable(sqlQuery.lastError().driverText());
        handleRestoreGameError();
        return;
    }

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the restore game query:" << qPrintable(sqlQuery.lastError().driverText());
        handleRestoreGameError();
        return;
    }

    if (!sqlQuery.first()) {
        qWarning() << "Failed to restore game. Game not found";
        handleRestoreGameError();
        return;
    }

    bool ok = false;

    m_gameId = sqlQuery.value(0).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the game id of the restored game";
        handleRestoreGameError();
        return;
    }

    const int rows = sqlQuery.value(1).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the rows count of the restored game";
        handleRestoreGameError();
        return;
    }

    const int columns = sqlQuery.value(2).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the columns count of the restored game";
        handleRestoreGameError();
        return;
    }

    m_turnId = sqlQuery.value(3).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the turn id of the restored game";
        handleRestoreGameError();
        return;
    }

    const int score = sqlQuery.value(4).toInt(&ok);
    if (!ok) {
        qWarning() << "Failed to get the score of the restored game";
        handleRestoreGameError();
        return;
    }

    const int bestScore = sqlQuery.value(5).toInt(&ok);
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

    m_db.commit();
    emit gameRestored({ rows, columns, score, bestScore, tiles });
}


int StorageWorker::databaseVersion()
{
    QSqlQuery query(m_db);

    if (!query.exec(QLatin1Literal("PRAGMA user_version"))) {
        qWarning() << "Failed to get database version:" << qPrintable(query.lastError().text());
        return WRONG_DATABASE_VERSION;
    }

    if (!query.first()) {
        qWarning() << "Failed to get database version: database version not found";
        return WRONG_DATABASE_VERSION;
    }

    bool ok = false;
    const int version = query.value(0).toInt(&ok);

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

    m_db.transaction();

    if (!executeFileQueries(QLatin1Literal("://sql/tables.sql"))) {
        m_db.rollback();
        return false;
    }

    m_db.commit();
    return true;
}


bool StorageWorker::executeFileQueries(const QString& fileName)
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
            qWarning() << "Failed to execute query:" << sqlQuery.lastError().driverText();
            return false;
        }
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
        qWarning() << "Failed to prepare the save tiles query:" << qPrintable(sqlQuery.lastError().driverText());
        return false;
    }

    for (const auto &tile : tiles) {
        sqlQuery.addBindValue(m_turnId);
        sqlQuery.addBindValue(tile.id());
        sqlQuery.addBindValue(tile.cell());
        sqlQuery.addBindValue(tile.value());

        if (!sqlQuery.exec()) {
            qWarning() << "Failed to execute the save tiles query:" << qPrintable(sqlQuery.lastError().driverText());
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
                                          "WHERE turn_id = ?");

    if (!sqlQuery.prepare(query)) {
        qWarning() << "Failed to prepare the restore tiles query:" << qPrintable(sqlQuery.lastError().driverText());
        return false;
    }

    sqlQuery.addBindValue(m_turnId);

    if (!sqlQuery.exec()) {
        qWarning() << "Failed to execute the restore tiles query:" << qPrintable(sqlQuery.lastError().driverText());
        return false;
    }

    if (!sqlQuery.first()) {
        qWarning() << "Failed to restore tiles. Tiles not found";
        return false;
    }

    bool ok = false;

    do {
        const int id = sqlQuery.value(0).toInt(&ok);
        if (!ok) {
            qWarning() << "Failed to get the tile id of the restored tile";
            return false;
        }

        const int cell = sqlQuery.value(1).toInt(&ok);
        if (!ok) {
            qWarning() << "Failed to get the cell index of the restored tile";
            return false;
        }

        const int value = sqlQuery.value(2).toInt(&ok);
        if (!ok) {
            qWarning() << "Failed to get the tile value of the restored tile";
            return false;
        }

        tiles.append({ id, cell, value });
    } while (sqlQuery.next());

    return true;
}


int StorageWorker::moveDirection(MoveDirection direction) const
{
    switch (direction) {
    case MoveDirection::None:
        return MOVE_DIRECTION_NONE;
    case MoveDirection::Left:
        return MOVE_DIRECTION_LEFT;
    case MoveDirection::Right:
        return MOVE_DIRECTION_RIGHT;
    case MoveDirection::Up:
        return MOVE_DIRECTION_UP;
    case MoveDirection::Down:
        return MOVE_DIRECTION_DOWN;
    }
}


void StorageWorker::handleCreateGameError()
{
    if (m_db.transaction()) {
        m_db.rollback();
    }

    m_gameId = WRONG_ID;
    emit createGameError();
}


void StorageWorker::handleSaveGameError()
{
    if (m_db.transaction()) {
        m_db.rollback();
    }

    m_turnId = WRONG_ID;
    emit saveGameError();
}


void StorageWorker::handleRestoreGameError()
{
    if (m_db.transaction()) {
        m_db.rollback();
    }

    m_gameId = WRONG_ID;
    m_turnId = WRONG_ID;
    emit restoreGameError();
}

} // namespace Internal
} // namespace Game
