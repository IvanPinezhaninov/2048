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


#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>

#include <memory>

#include "movedirection.h"


namespace Game {
namespace Internal {
class GameControllerPrivate;
} // namespace Internal

using MoveDirection = Internal::MoveDirection;

class GameController final : public QObject
{
    Q_OBJECT
public:
    explicit GameController(QObject *parent = nullptr);
    ~GameController();

    bool init();

public slots:
    void shutdown();

private slots:
    void onGameReady();
    void onStartNewGameRequested();
    void onContinueGameRequested();
    void onUndoRequested();
    void onMoveTilesRequested(MoveDirection direction);
    void onTileMoveFinished();
    void onStorageReady();
    void onStorageError();
    void onGameCreated(const QVariantMap &game);
    void onCreateGameError();
    void onGameRestored(const QVariantMap &game);
    void onRestoreGameError();
    void onTurnSaved();
    void onSaveTurnError();
    void onTurnUndid(const QVariantMap &turn);
    void onUndoTurnError();
    void startNewGame();
    void restoreGame();

private:
    Q_DISABLE_COPY(GameController)

    const std::unique_ptr<Internal::GameControllerPrivate> d;

    friend class Internal::GameControllerPrivate;
};

} // namespace Game

#endif // GAMECONTROLLER_H
