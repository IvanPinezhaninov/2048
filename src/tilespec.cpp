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


namespace Game {
namespace Internal {

class TileSpecPrivate
{
public:
    TileSpecPrivate(int id, int cell, int value);

    int m_id;
    int m_cell;
    int m_value;
};


TileSpecPrivate::TileSpecPrivate(int id, int cell, int value) :
    m_id(id),
    m_cell(cell),
    m_value(value)
{
}


TileSpec::TileSpec(int id, int cell, int value) :
    d(std::make_shared<TileSpecPrivate>(id, cell, value))
{
}


int TileSpec::id() const
{
    return d->m_id;
}


int TileSpec::cell() const
{
    return d->m_cell;
}


int TileSpec::value() const
{
    return d->m_value;
}

} // namespace Internal
} // namespace Game
