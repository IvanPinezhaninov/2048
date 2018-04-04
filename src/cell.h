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


#ifndef CELL_H
#define CELL_H

#include <QObject>

#include <memory>

QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE

class Tile;


class Cell final : public QObject
{
    Q_OBJECT
public:
    explicit Cell(QQuickItem *cellQuickItem, QObject *parent = nullptr);
    ~Cell();

    std::shared_ptr<Tile> tile() const;

public slots:
    void setTile(const std::shared_ptr<Tile> &tile);

private slots:
    void onXChanged();
    void onYChanged();
    void onWidthChanged();
    void onHeightChanged();

private:
    Q_DISABLE_COPY(Cell)

    QQuickItem *const m_cellQuickItem;
    std::weak_ptr<Tile> m_tile;
};

#endif // CELL_H
