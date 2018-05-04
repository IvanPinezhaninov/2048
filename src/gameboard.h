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


#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QObject>

#include <memory>

QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE


namespace Game {
namespace Internal {

class Cell;
class GameboardSize;
class GameboardPrivate;

using Cell_ptr = std::shared_ptr<Cell>;

class Gameboard final : public QObject
{
    Q_OBJECT
public:
    explicit Gameboard(QQuickItem *gameboardQuickItem, QObject *parent = nullptr);
    ~Gameboard();

    int rows() const;
    int columns() const;
    QList<Cell_ptr> cells() const;

    QQuickItem *tilesParent() const;

signals:
    void sizeChanged();
    void rowsChanged(int rows);
    void columnsChanged(int columns);
    void cellsChanged();

public slots:
    void setRows(int rows);
    void setColumns(int columns);
    void setSize(int rows, int columns);

private slots:
    void onRowsChanged(int rows);
    void onColumnsChanged(int columns);
    void onCellItemAdded(int index, QQuickItem *item);
    void onCellItemRemoved(int index, QQuickItem *item);

private:
    Q_DISABLE_COPY(Gameboard)

    const std::unique_ptr<GameboardPrivate> d;

    friend class GameboardPrivate;
};

} // namespace Internal
} // namespace Game

#endif // GAMEBOARD_H
