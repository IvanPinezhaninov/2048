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
static const char *const CELLS_REPEATER_OBJECT_NAME = "CellsRepeater";

static const char *const ROWS_PROPERTY_NAME = "rows";
static const char *const COLUMNS_PROPERTY_NAME = "columns";
static const char *const DEBUG_PROPERTY_NAME = "debug";


namespace Game {
namespace Internal {

class GameboardPrivate final
{
public:
    GameboardPrivate(QQuickItem *gameboardItem);

    QQuickItem *const m_gameboardItem;
    QQuickItem *const m_gridItem;
    QQuickItem *const m_repeaterItem;

    int m_rows;
    int m_columns;
    QMap<int, Cell_ptr> m_cells;
};


GameboardPrivate::GameboardPrivate(QQuickItem *gameboardItem) :
    m_gameboardItem(gameboardItem),
    m_gridItem(q_check_ptr(gameboardItem->findChild<QQuickItem*>(QLatin1Literal(CELLS_GRID_OBJECT_NAME)))),
    m_repeaterItem(q_check_ptr(gameboardItem->findChild<QQuickItem*>(QLatin1Literal(CELLS_REPEATER_OBJECT_NAME)))),
    m_rows(0),
    m_columns(0)
{
#ifdef QT_DEBUG
  m_gameboardItem->setProperty(DEBUG_PROPERTY_NAME, true);
#endif
}


Gameboard::Gameboard(QQuickItem *gameboardItem, QObject *parent) :
    QObject(parent),
    d(std::make_unique<GameboardPrivate>(gameboardItem))
{
    connect(this, &Gameboard::rowsChanged, this, &Gameboard::onRowsChanged);
    connect(this, &Gameboard::columnsChanged, this, &Gameboard::onColumnsChanged);
    connect(d->m_repeaterItem, SIGNAL(itemAdded(int,QQuickItem*)), this, SLOT(onCellItemAdded(int,QQuickItem*)));
    connect(d->m_repeaterItem, SIGNAL(itemRemoved(int,QQuickItem*)), this, SLOT(onCellItemAdded(int,QQuickItem*)));
}


Gameboard::~Gameboard()
{
}


int Gameboard::rows() const
{
    return d->m_rows;
}


int Gameboard::columns() const
{
    return d->m_columns;
}


QList<Cell_ptr> Gameboard::cells() const
{
    return d->m_cells.values();
}


QQuickItem *Gameboard::tilesParent() const
{
    return d->m_gridItem;
}


void Gameboard::setRows(int rows)
{
    if (d->m_rows != rows) {
        d->m_rows = rows;
        emit rowsChanged(rows);
        emit sizeChanged();
    }
}


void Gameboard::setColumns(int columns)
{
    if (d->m_columns != columns) {
        d->m_columns = columns;
        emit columnsChanged(columns);
        emit sizeChanged();
    }
}


void Gameboard::setSize(int rows, int columns)
{
    bool changed = false;

    if (d->m_rows != rows) {
        d->m_rows = rows;
        changed = true;
        emit rowsChanged(rows);
    }

    if (d->m_columns != columns) {
        d->m_columns = columns;
        changed = true;
        emit columnsChanged(columns);
    }

    if (changed) {
        emit sizeChanged();
    }
}

void Gameboard::onRowsChanged(int rows)
{
    Q_ASSERT(nullptr != d->m_gameboardItem);

    d->m_gameboardItem->setProperty(ROWS_PROPERTY_NAME, rows);
}


void Gameboard::onColumnsChanged(int columns)
{
    Q_ASSERT(nullptr != d->m_gameboardItem);

    d->m_gameboardItem->setProperty(COLUMNS_PROPERTY_NAME, columns);
}


void Gameboard::onCellItemAdded(int index, QQuickItem *cellItem)
{
    d->m_cells.insert(index, std::make_shared<Cell>(index, cellItem));
    emit cellsChanged();
}


void Gameboard::onCellItemRemoved(int index, QQuickItem *cellItem)
{
    Q_UNUSED(cellItem)

    d->m_cells.remove(index);
    emit cellsChanged();
}

} // namespace Internal
} // namespace Game
