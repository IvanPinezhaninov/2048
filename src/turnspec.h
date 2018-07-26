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


#ifndef TURNSPEC_H
#define TURNSPEC_H

#include <memory>

#include <QList>
#include <QMetaType>

#include "gamestate.h"
#include "movedirection.h"


namespace Game {
namespace Internal {

class TileSpec;
class TurnSpecPrivate;

class TurnSpec final
{
public:
    TurnSpec();
    TurnSpec(int gameId, int turnId, int score, int bestScore, GameState gameState,
             MoveDirection moveDirection, const QList<TileSpec> &tiles = QList<TileSpec>());

    int gameId() const;
    int turnId() const;
    int score() const;
    int bestScore() const;
    GameState gameState() const;
    MoveDirection moveDirection() const;
    QList<TileSpec> tiles() const;

private:
    std::shared_ptr<TurnSpecPrivate> d;
};

} // namespace Internal
} // namespace Game

Q_DECLARE_METATYPE(Game::Internal::TurnSpec)

#endif // TURNSPEC_H
