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


#ifndef TILE_H
#define TILE_H

#include <QObject>

#include <memory>

QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE

namespace Game {
namespace Internal {

class Cell;

using Cell_ptr = std::shared_ptr<Cell>;

class Tile final : public QObject, public std::enable_shared_from_this<Tile>
{
    Q_OBJECT
public:
    Tile(int id, int value, QQuickItem *tileItem, QQuickItem *parent);
    ~Tile();

    int id() const;
    void setId(int id);

    int value() const;
    void setValue(int value);

    void hide(bool animation = true);
    void show(bool animation = true);

    Cell_ptr cell() const;
    void setCell(const Cell_ptr &cell);

    qreal z() const;
    void setZ(qreal z);

signals:
    void moveFinished();

private slots:
    void onCellXChanged(qreal x);
    void onCellYChanged(qreal y);
    void onCellWidthChanged(qreal width);
    void onCellHeightChanged(qreal height);
    void onMoveFinished();

private:
    Q_DISABLE_COPY(Tile)

    void move(const QRectF &location);

    const std::unique_ptr<QQuickItem> m_tileItem;
    int m_id;
    int m_value;
    std::weak_ptr<Cell> m_cell;
};

} // namespace Internal
} // namespace Game

#endif // TILE_H
