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


#ifndef STORAGEWORKER_H
#define STORAGEWORKER_H

#include <QMutex>
#include <QObject>
#include <QSqlDatabase>

#include "gamestate.h"
#include "movedirection.h"


namespace Game {
namespace Internal {

class StorageWorker final : public QObject
{
    Q_OBJECT
public:
    StorageWorker();

signals:
    void storageReady();
    void storageError();

    void gameCreated(const QVariantMap &game);
    void createGameError();

    void turnSaved();
    void saveTurnError();

    void gameRestored(const QVariantMap &game);
    void restoreGameError();

    void turnUndid(const QVariantMap &turn);
    void undoTurnError();

public slots:
    void openDatabase();
    void closeDatabase();
    void createGame(int rows, int columns);
    void restoreGame();
    void saveTurn(const QVariantMap &turn);
    void undoTurn(int turnId);

private:
    Q_DISABLE_COPY(StorageWorker)

    int databaseVersion();
    bool createDatabase();
    bool executeQuery(const QString &query, QString &error);
    bool executeFileQueries(const QString &fileName);

    bool finishGame();
    bool createGame(int rows, int columns, QVariant &gameId);

    bool saveGameState(const QVariant &gameId, GameState state);
    bool saveTiles(const QVariant &turnId, const QVariantList &tiles);
    QVariantList restoreTiles(const QVariant &turnId, bool &ok);
    QVariant getMaxTurnId(bool &ok) const;

    void removeTurns();
    void removeTiles();
    void vacuum();

    int gameStateToInt(GameState gameState) const;
    QString gameStateName(const QVariant &gameState) const;
    QString gameStateName(GameState gameState) const;

    int moveDirectionToInt(const QVariant &moveDirection) const;
    int moveDirectionToInt(MoveDirection moveDirection) const;
    QString moveDirectionName(const QVariant &moveDirection) const;
    QString moveDirectionName(MoveDirection moveDirection) const;

    QString tilesToString(const QVariantList &tiles) const;

    void handleCreateGameError(bool rollback = true);
    void handleRestoreGameError(bool rollback = true);
    void handleSaveTurnError(bool rollback = true);
    void handleUndoTurnError(bool rollback = true);

    bool startTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

    QSqlDatabase m_db;
    QMutex m_lock;
};

} // namespace Internal
} // namespace Game

#endif // STORAGEWORKER_H
