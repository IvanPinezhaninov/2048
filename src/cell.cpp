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

Cell::Cell(QQuickItem *cellQuickItem, QObject *parent) :
    QObject(parent),
    m_cellQuickItem(cellQuickItem)
{
    Q_ASSERT(m_cellQuickItem != nullptr);

    connect(m_cellQuickItem, &QQuickItem::xChanged, this, &Cell::onXChanged);
    connect(m_cellQuickItem, &QQuickItem::yChanged, this, &Cell::onYChanged);
    connect(m_cellQuickItem, &QQuickItem::widthChanged, this, &Cell::onWidthChanged);
    connect(m_cellQuickItem, &QQuickItem::heightChanged, this, &Cell::onHeightChanged);
}


Cell::~Cell()
{
}


Tile_ptr Cell::tile() const
{
    return m_tile.lock();
}


qreal Cell::x() const
{
    return m_cellQuickItem->x();
}


qreal Cell::y() const
{
    return m_cellQuickItem->y();
}


qreal Cell::width() const
{
    return m_cellQuickItem->width();
}


qreal Cell::height() const
{
    return m_cellQuickItem->height();
}


void Cell::setTile(const Tile_ptr &tile)
{
    Q_ASSERT(m_cellQuickItem != nullptr);

    m_tile = tile;

    if (const auto &tile = m_tile.lock()) {
        const auto &cell = shared_from_this();
        if (tile->cell() != cell) {
            tile->setCell(cell);
            tile->move({ x(), y(), width(), height() });
        }
    }
}


void Cell::onXChanged()
{
    Q_ASSERT(m_cellQuickItem);

    if (const auto &tile = m_tile.lock()) {
        tile->setX(x());
    }
}


void Cell::onYChanged()
{
    Q_ASSERT(m_cellQuickItem);

    if (const auto &tile = m_tile.lock()) {
        tile->setY(y());
    }
}


void Cell::onWidthChanged()
{
    Q_ASSERT(m_cellQuickItem);

    if (const auto &tile = m_tile.lock()) {
        tile->setWidth(width());
    }
}


void Cell::onHeightChanged()
{
    Q_ASSERT(m_cellQuickItem);

    if (const auto &tile = m_tile.lock()) {
        tile->setHeight(height());
    }
}

} // namespace Game
