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


#ifndef GAMEBOARDSIZE_H
#define GAMEBOARDSIZE_H

namespace Game {
namespace Internal {

class GameboardSize
{
public:
    GameboardSize();
    GameboardSize(int rows, int columns);

    bool operator==(const GameboardSize &other) const;
    bool operator!=(const GameboardSize &other) const;

    void setRows(int rows);
    int rows() const;

    void setColumns(int columns);
    int columns() const;

private:
    int m_rows;
    int m_columns;
};

} // namespace Internal
} // namespace Game

#endif // GAMEBOARDSIZE_H
