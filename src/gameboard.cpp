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
#include "gameboard.h"

#include <QQuickItem>

static const char *const CELLS_GRID_OBJECT_NAME = "CellsGrid";

static const char *const ROWS_PROPERTY_NAME = "rows";
static const char *const COLUMNS_PROPERTY_NAME = "columns";
static const char *const CELLS_COUNT_PROPERTY_NAME = "cellsCount";
static const char *const GET_CELL_FUNCTION_NAME = "getCell";

class GameboardPrivate
{
public:
    GameboardPrivate(QQuickItem *gameboardQuickItem);

    QQuickItem *const m_gameboardQuickItem;
    QQuickItem *const m_cellsGridQuickItem;
};


GameboardPrivate::GameboardPrivate(QQuickItem *gameboardQuickItem) :
    m_gameboardQuickItem(gameboardQuickItem),
    m_cellsGridQuickItem(q_check_ptr(gameboardQuickItem->findChild<QQuickItem*>(QLatin1Literal(CELLS_GRID_OBJECT_NAME))))
{
}


Gameboard::Gameboard(QQuickItem *gameboardQuickItem, QObject *parent) :
    QObject(parent),
    d(std::make_unique<GameboardPrivate>(gameboardQuickItem))
{
}


Gameboard::~Gameboard()
{
}


QQuickItem *Gameboard::tilesParent() const
{
    return d->m_cellsGridQuickItem;
}


int Gameboard::rows() const
{
    Q_ASSERT(nullptr != d->m_gameboardQuickItem);

    return d->m_gameboardQuickItem->property(ROWS_PROPERTY_NAME).toInt();
}


int Gameboard::columns() const
{
    Q_ASSERT(nullptr != d->m_gameboardQuickItem);

    return d->m_gameboardQuickItem->property(COLUMNS_PROPERTY_NAME).toInt();
}


QMap<int, std::shared_ptr<Cell>> Gameboard::cells() const
{
    Q_ASSERT(nullptr != d->m_gameboardQuickItem);

    QMap<int, std::shared_ptr<Cell>> cells;
    const int cellsCount = d->m_gameboardQuickItem->property(CELLS_COUNT_PROPERTY_NAME).toInt();

    for (int i = 0; i < cellsCount; ++i) {
        QVariant returnValue;
        QMetaObject::invokeMethod(d->m_gameboardQuickItem, GET_CELL_FUNCTION_NAME,
                                  Q_RETURN_ARG(QVariant, returnValue), Q_ARG(QVariant, i));
        QQuickItem *cellQuickItem = q_check_ptr(returnValue.value<QQuickItem*>());
        cells.insert(i, std::make_shared<Cell>(cellQuickItem));
    }

    return cells;
}


void Gameboard::setRows(int rows)
{
    Q_ASSERT(nullptr != d->m_gameboardQuickItem);

    d->m_gameboardQuickItem->setProperty(ROWS_PROPERTY_NAME, rows);
}


void Gameboard::setColumns(int columns)
{
    Q_ASSERT(nullptr != d->m_gameboardQuickItem);

    d->m_gameboardQuickItem->setProperty(COLUMNS_PROPERTY_NAME, columns);
}
