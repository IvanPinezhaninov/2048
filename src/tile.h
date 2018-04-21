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
class QQmlComponent;
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
    explicit Tile(int id, QQmlComponent *tileQmlComponent, QQuickItem *parent);
    ~Tile();

    int id() const;
    void setId(int id);

    int value() const;
    void setValue(int value);
    void resetValue();

    Cell_ptr cell() const;
    void setCell(const Cell_ptr &cell);

    qreal x() const;
    qreal y() const;
    qreal z() const;
    qreal width() const;
    qreal height() const;

signals:
    void moveFinished();

public slots:
    void move(const QRectF &location);

    void setX(qreal x);
    void setY(qreal y);
    void setZ(qreal z);
    void setWidth(qreal width);
    void setHeight(qreal height);

private slots:
    void onMoveFinished();

private:
    Q_DISABLE_COPY(Tile)

    const std::unique_ptr<QQuickItem> m_tileQuickItem;
    int m_id;
    int m_value;
    std::weak_ptr<Cell> m_cell;
};

} // namespace Internal
} // namespace Game

#endif // TILE_H
