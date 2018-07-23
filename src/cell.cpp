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


#include "cell.h"
#include "tile.h"

#include <QQuickItem>


namespace Game {
namespace Internal {

Cell::Cell(int index, QQuickItem *cellItem, QObject *parent) :
    QObject(parent),
    m_index(index),
    m_cellItem(cellItem)
{
    connect(m_cellItem, &QQuickItem::xChanged, this, &Cell::onXChanged);
    connect(m_cellItem, &QQuickItem::yChanged, this, &Cell::onYChanged);
    connect(m_cellItem, &QQuickItem::widthChanged, this, &Cell::onWidthChanged);
    connect(m_cellItem, &QQuickItem::heightChanged, this, &Cell::onHeightChanged);
}


int Cell::index() const
{
    return m_index;
}


Tile_ptr Cell::tile() const
{
    return m_tile.lock();
}


qreal Cell::x() const
{
    return m_cellItem->x();
}


qreal Cell::y() const
{
    return m_cellItem->y();
}


qreal Cell::width() const
{
    return m_cellItem->width();
}


qreal Cell::height() const
{
    return m_cellItem->height();
}


void Cell::setTile(const Tile_ptr &tile)
{
    if (const auto &tile = m_tile.lock()) {
        tile->setCell(nullptr);
    }

    if (tile) {
        m_tile = tile;
        const auto &cell = shared_from_this();
        tile->setCell(cell);
    } else {
        m_tile.reset();
    }
}


void Cell::onXChanged()
{
    emit xChanged(m_cellItem->x());
}


void Cell::onYChanged()
{
    emit yChanged(m_cellItem->y());
}


void Cell::onWidthChanged()
{
    emit widthChanged(m_cellItem->width());
}


void Cell::onHeightChanged()
{
    emit heightChanged(m_cellItem->height());
}

} // namespace Internal
} // namespace Game
