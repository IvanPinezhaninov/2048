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


#include "tilespec.h"
#include "turnspec.h"

static const int WRONG_ID = -1;


namespace Game {
namespace Internal {

class TurnSpecPrivate final
{
public:
    TurnSpecPrivate(int gameId, int turnId, int score, int bestScore, GameState gameState,
                    MoveDirection moveDirection, const QList<TileSpec> &tiles = QList<TileSpec>());

    int m_gameId;
    int m_turnId;
    int m_score;
    int m_bestScore;
    GameState m_gameState;
    MoveDirection m_moveDirection;
    QList<TileSpec> m_tiles;
};


TurnSpecPrivate::TurnSpecPrivate(int gameId, int turnId, int score, int bestScore, GameState gameState,
                                 MoveDirection moveDirection, const QList<TileSpec> &tiles) :
    m_gameId(gameId),
    m_turnId(turnId),
    m_score(score),
    m_bestScore(bestScore),
    m_gameState(gameState),
    m_moveDirection(moveDirection),
    m_tiles(tiles)
{
}


TurnSpec::TurnSpec() :
    d(new TurnSpecPrivate(WRONG_ID, WRONG_ID, 0, 0, GameState::Init, MoveDirection::None))
{
}


TurnSpec::TurnSpec(int gameId, int turnId, int score, int bestScore, GameState state,
                   MoveDirection direction, const QList<TileSpec> &tiles) :
    d(new TurnSpecPrivate(gameId, turnId, score, bestScore, state, direction, tiles))
{
}


int TurnSpec::gameId() const
{
    return d->m_gameId;
}


int TurnSpec::turnId() const
{
    return d->m_turnId;
}


int TurnSpec::score() const
{
    return d->m_score;
}


int TurnSpec::bestScore() const
{
    return d->m_bestScore;
}


GameState TurnSpec::gameState() const
{
    return d->m_gameState;
}


MoveDirection TurnSpec::moveDirection() const
{
    return d->m_moveDirection;
}


QList<TileSpec> TurnSpec::tiles() const
{
    return d->m_tiles;
}

} // namespace Internal
} // namespace Game
