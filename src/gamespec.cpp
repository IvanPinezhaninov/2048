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


#include <QList>

#include "gamespec.h"
#include "tilespec.h"

static const int WRONG_ID = -1;


namespace Game {
namespace Internal {

class GameSpecPrivate final
{
public:
    GameSpecPrivate(int gameId, int rows, int columns, int turnId, int score, int bestScore,
                    GameState gameState, const QList<TileSpec> &tiles = QList<TileSpec>());

    int m_gameId;
    int m_rows;
    int m_columns;
    int m_turnId;
    int m_score;
    int m_bestScore;
    GameState m_gameState;
    QList<TileSpec> m_tiles;
};


GameSpecPrivate::GameSpecPrivate(int gameId, int rows, int columns, int turnId, int score, int bestScore,
                                 GameState gameState, const QList<TileSpec> &tiles) :
    m_gameId(gameId),
    m_rows(rows),
    m_columns(columns),
    m_turnId(turnId),
    m_score(score),
    m_bestScore(bestScore),
    m_gameState(gameState),
    m_tiles(tiles)
{
}


GameSpec::GameSpec() :
    GameSpec(WRONG_ID, 0, 0, WRONG_ID, 0, 0, GameState::Init)
{
}


GameSpec::GameSpec(int gameId, int rows, int columns, int turnId) :
    GameSpec(gameId, rows, columns, turnId, 0, 0, GameState::Init)
{
}


GameSpec::GameSpec(int gameId, int rows, int columns, int turnId, int score, int bestScore,
                   GameState gameState, const QList<TileSpec> &tiles) :
    d(new GameSpecPrivate(gameId, rows, columns, turnId, score, bestScore, gameState, tiles))
{
}


int GameSpec::gameId() const
{
    return d->m_gameId;
}


int GameSpec::rows() const
{
    return d->m_rows;
}


int GameSpec::columns() const
{
    return d->m_columns;
}


int GameSpec::turnId() const
{
    return d->m_turnId;
}


int GameSpec::score() const
{
    return d->m_score;
}


int GameSpec::bestScore() const
{
    return d->m_bestScore;
}


GameState GameSpec::gameState() const
{
    return d->m_gameState;
}


QList<TileSpec> GameSpec::tiles() const
{
    return d->m_tiles;
}

} // namespace Internal
} // namespace Game
