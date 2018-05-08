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


namespace Game {
namespace Internal {

class TurnSpecPrivate
{
public:
    TurnSpecPrivate(MoveDirection moveDirection, GameState gameState,
                    int score, int bestScore, const QList<TileSpec> &tiles);
    ~TurnSpecPrivate();

    int m_score;
    int m_bestScore;
    QList<TileSpec> m_tiles;
    MoveDirection m_moveDirection;
    GameState m_gameState;
};


TurnSpecPrivate::TurnSpecPrivate(MoveDirection moveDirection, GameState gameState,
                                 int score, int bestScore, const QList<TileSpec> &tiles) :
    m_score(score),
    m_bestScore(bestScore),
    m_tiles(tiles),
    m_moveDirection(moveDirection),
    m_gameState(gameState)
{
}


TurnSpecPrivate::~TurnSpecPrivate()
{
}


TurnSpec::TurnSpec() :
    TurnSpec(MoveDirection::None, GameState::Init, 0, 0, {})
{
}


TurnSpec::TurnSpec(MoveDirection moveDirection, GameState gameState,
                   int score, int bestScore, const QList<TileSpec> &tiles) :
    d(std::make_shared<TurnSpecPrivate>(moveDirection, gameState, score, bestScore, tiles))
{
}


MoveDirection TurnSpec::moveDirection() const
{
    return d->m_moveDirection;
}


GameState TurnSpec::gameState() const
{
    return d->m_gameState;
}


int TurnSpec::score() const
{
    return d->m_score;
}


int TurnSpec::bestScore() const
{
    return d->m_bestScore;
}


QList<TileSpec> TurnSpec::tiles() const
{
    return d->m_tiles;
}

} // namespace Internal
} // namespace Game
