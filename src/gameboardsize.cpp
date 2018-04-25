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


#include "gameboardsize.h"

namespace Game {
namespace Internal {

GameboardSize::GameboardSize() :
    GameboardSize(0, 0)
{
}


GameboardSize::GameboardSize(int rows, int columns) :
    m_rows(rows),
    m_columns(columns)
{
}


bool GameboardSize::operator==(const GameboardSize &other) const
{
  return (m_rows == other.m_rows && m_columns == other.m_columns);
}


bool GameboardSize::operator!=(const GameboardSize &other) const
{
  return !operator==(other);
}


void GameboardSize::setRows(int rows)
{
    m_rows = rows;
}


int GameboardSize::rows() const
{
    return m_rows;
}


void GameboardSize::setColumns(int columns)
{
    m_columns = columns;
}


int GameboardSize::columns() const
{
    return m_columns;
}

} // namespace Internal
} // namespace Game
