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


#include "tile.h"

#include <QRectF>
#include <QQmlComponent>
#include <QQuickItem>

static const char *const X_PROPERTY_NAME = "x";
static const char *const Y_PROPERTY_NAME = "y";
static const char *const WIDTH_PROPERTY_NAME = "width";
static const char *const HEIGHT_PROPERTY_NAME = "height";
static const char *const VALUE_PROPERTY_NAME = "value";


Tile::Tile(QQmlComponent *tileQmlComponent, QQuickItem *parent) :
    QObject(parent),
    m_tileQuickItem(qobject_cast<QQuickItem*>(tileQmlComponent->create())),
    m_value(0)
{
    m_tileQuickItem->setParentItem(parent);
    connect(m_tileQuickItem.get(), SIGNAL(moveFinished()), this, SLOT(onMoveFinished()));
}


Tile::~Tile()
{
}


qreal Tile::x() const
{
    return m_tileQuickItem->x();
}


qreal Tile::y() const
{
    return m_tileQuickItem->y();
}


qreal Tile::z() const
{
    return m_tileQuickItem->z();
}


qreal Tile::width() const
{
    return m_tileQuickItem->width();
}


qreal Tile::height() const
{
    return m_tileQuickItem->height();
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


void Tile::move(const QRectF &location)
{
    if (!qFuzzyCompare(m_tileQuickItem->property(X_PROPERTY_NAME).toReal(), location.x())) {
        m_tileQuickItem->setProperty(X_PROPERTY_NAME, location.x());
    }

    if (!qFuzzyCompare(m_tileQuickItem->property(Y_PROPERTY_NAME).toReal(), location.y())) {
        m_tileQuickItem->setProperty(Y_PROPERTY_NAME, location.y());
    }

    if (!qFuzzyCompare(m_tileQuickItem->property(WIDTH_PROPERTY_NAME).toReal(), location.width())) {
        m_tileQuickItem->setProperty(WIDTH_PROPERTY_NAME, location.width());
    }

    if (!qFuzzyCompare(m_tileQuickItem->property(HEIGHT_PROPERTY_NAME).toReal(), location.height())) {
        m_tileQuickItem->setProperty(HEIGHT_PROPERTY_NAME, location.height());
    }
}


void Tile::setX(qreal x)
{
    m_tileQuickItem->setX(x);
}


void Tile::setY(qreal y)
{
    m_tileQuickItem->setY(y);
}


void Tile::setZ(qreal z)
{
    m_tileQuickItem->setZ(z);
}


void Tile::setWidth(qreal width)
{
    m_tileQuickItem->setWidth(width);
}


void Tile::setHeight(qreal height)
{
    m_tileQuickItem->setHeight(height);
}


void Tile::onMoveFinished()
{
    m_tileQuickItem->setProperty(VALUE_PROPERTY_NAME, m_value);
    emit moveFinished();
}
