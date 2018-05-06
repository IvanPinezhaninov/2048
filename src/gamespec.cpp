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
#include "tilespec.h"


namespace Game {
namespace Internal {

class GameSpecPrivate
{
public:
    GameSpecPrivate(int rows, int columns, int score, int bestScore, const QList<TileSpec> &tiles);
    ~GameSpecPrivate();

    int m_rows;
    int m_columns;
    int m_score;
    int m_bestScore;
    QList<TileSpec> m_tiles;
};


GameSpecPrivate::GameSpecPrivate(int rows, int columns, int score, int bestScore, const QList<TileSpec> &tiles) :
    m_rows(rows),
    m_columns(columns),
    m_score(score),
    m_bestScore(bestScore),
    m_tiles(tiles)
{
}


GameSpecPrivate::~GameSpecPrivate()
{
}


GameSpec::GameSpec() :
    GameSpec(0, 0, 0, 0, {})
{
}


GameSpec::GameSpec(int rows, int columns, int score, int bestScore, const QList<TileSpec> &tiles) :
    d(std::make_shared<GameSpecPrivate>(rows, columns, score, bestScore, tiles))
{
}


int GameSpec::rows() const
{
    return d->m_rows;
}


int GameSpec::columns() const
{
    return d->m_columns;
}


int GameSpec::score() const
{
    return d->m_score;
}


int GameSpec::bestScore() const
{
    return d->m_bestScore;
}


QList<TileSpec> GameSpec::tiles() const
{
    return d->m_tiles;
}

} // namespace Internal
} // namespace Game
