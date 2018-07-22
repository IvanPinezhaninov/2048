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

#include <QRectF>
#include <QQmlComponent>
#include <QQuickItem>

static const char *const X_PROPERTY_NAME = "x";
static const char *const Y_PROPERTY_NAME = "y";
static const char *const WIDTH_PROPERTY_NAME = "width";
static const char *const HEIGHT_PROPERTY_NAME = "height";
static const char *const VALUE_PROPERTY_NAME = "value";


namespace Game {
namespace Internal {

Tile::Tile(int id, QQmlComponent *tileQmlComponent, QQuickItem *parent, bool animation) :
    QObject(parent),
    m_tileQuickItem(qobject_cast<QQuickItem*>(tileQmlComponent->create())),
    m_id(id),
    m_value(0)
{
    if (!animation) {
        m_tileQuickItem->setProperty(VALUE_PROPERTY_NAME, -1);
    }

    m_tileQuickItem->setParentItem(parent);
    connect(m_tileQuickItem.get(), SIGNAL(moveFinished()), this, SLOT(onMoveFinished()));
}


Tile::~Tile()
{
}


int Tile::id() const
{
    return m_id;
}


void Tile::setId(int id)
{
    m_id = id;
}


int Tile::value() const
{
    return m_value;
}


void Tile::setValue(int value)
{
    if (0 == m_value) {
        m_tileQuickItem->setProperty(VALUE_PROPERTY_NAME, value);
    }

    m_value = value;
}


void Tile::resetValue()
{
    m_value = 0;
    m_tileQuickItem->setProperty(VALUE_PROPERTY_NAME, 0);
}


Cell_ptr Tile::cell() const
{
    return m_cell.lock();
}


void Tile::setCell(const Cell_ptr &cell)
{
    if (cell) {
        const auto &tile = shared_from_this();
        if (cell->tile() == tile) {
            m_cell = cell;
            connect(cell.get(), &Cell::xChanged, this, &Tile::onCellXChanged);
            connect(cell.get(), &Cell::yChanged, this, &Tile::onCellYChanged);
            connect(cell.get(), &Cell::widthChanged, this, &Tile::onCellWidthChanged);
            connect(cell.get(), &Cell::heightChanged, this, &Tile::onCellHeightChanged);
            move({ cell->x(), cell->y(), cell->width(), cell->height() });
        } else {
            cell->setTile(tile);
        }
    } else {
        if (const auto &cell = m_cell.lock()) {
            disconnect(cell.get(), &Cell::xChanged, this, &Tile::onCellXChanged);
            disconnect(cell.get(), &Cell::yChanged, this, &Tile::onCellYChanged);
            disconnect(cell.get(), &Cell::widthChanged, this, &Tile::onCellWidthChanged);
            disconnect(cell.get(), &Cell::heightChanged, this, &Tile::onCellHeightChanged);
            m_cell.reset();
            cell->setTile(nullptr);
        }
    }
}


qreal Tile::z() const
{
    return m_tileQuickItem->z();
}


void Tile::onCellXChanged(qreal x)
{
    m_tileQuickItem->setX(x);
}


void Tile::onCellYChanged(qreal y)
{
    m_tileQuickItem->setY(y);
}


void Tile::setZ(qreal z)
{
    m_tileQuickItem->setZ(z);
}


void Tile::onCellWidthChanged(qreal width)
{
    m_tileQuickItem->setWidth(width);
}


void Tile::onCellHeightChanged(qreal height)
{
    m_tileQuickItem->setHeight(height);
}


void Tile::onMoveFinished()
{
    m_tileQuickItem->setProperty(VALUE_PROPERTY_NAME, m_value);
    emit moveFinished();
}


void Tile::move(const QRectF &location)
{
    m_tileQuickItem->setProperty(X_PROPERTY_NAME, location.x());
    m_tileQuickItem->setProperty(Y_PROPERTY_NAME, location.y());
    m_tileQuickItem->setProperty(WIDTH_PROPERTY_NAME, location.width());
    m_tileQuickItem->setProperty(HEIGHT_PROPERTY_NAME, location.height());
}

} // namespace Internal
} // namespace Game
