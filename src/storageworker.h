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
#include "tilespec.h"


namespace Game {
namespace Internal {

class GameSpec;
class TurnSpec;

using TileSpecs = QList<TileSpec>;

class StorageWorker final : public QObject
{
    Q_OBJECT
public:
    StorageWorker();

signals:
    void storageReady();
    void storageError();

    void gameCreated();
    void createGameError();

    void gameSaved();
    void saveGameError();

    void gameRestored(const GameSpec &game);
    void restoreGameError();

public slots:
    void openDatabase();
    void closeDatabase();
    void createGame(int rows, int columns);
    void restoreGame();
    void saveTurn(const TurnSpec &turn);

private:
    Q_DISABLE_COPY(StorageWorker)

    int databaseVersion();
    bool createDatabase();
    bool executeQuery(const QString &query, QString &error);
    bool executeFileQueries(const QString &fileName);

    bool updateGame();
    bool createGame(int rows, int columns, int &gameId);

    bool saveTiles(const TileSpecs &tiles);
    bool restoreTiles(TileSpecs &tiles);

    void clearTurns();
    void clearTiles();
    void vacuum();

    int gameStateToInt(GameState gameState) const;
    GameState gameStateFromInt(int value) const;

    int moveDirectionToInt(MoveDirection moveDirection) const;
    MoveDirection moveDirectionFromInt(int value) const;

    void handleCreateGameError();
    void handleSaveGameError();
    void handleRestoreGameError();

    QSqlDatabase m_db;
    QMutex m_lock;
    int m_gameId;
    int m_turnId;
};

} // namespace Internal
} // namespace Game

Q_DECLARE_METATYPE(Game::Internal::TileSpecs);

#endif // STORAGEWORKER_H
