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


namespace Game {
namespace Internal {

class Tile;

using Tile_ptr = std::shared_ptr<Tile>;

class Cell final : public QObject, public std::enable_shared_from_this<Cell>
{
    Q_OBJECT
public:
    Cell(int index, QQuickItem *cellItem, QObject *parent = nullptr);

    int index() const;
    Tile_ptr tile() const;

    qreal x() const;
    qreal y() const;
    qreal width() const;
    qreal height() const;

signals:
    void xChanged(qreal x);
    void yChanged(qreal y);
    void widthChanged(qreal width);
    void heightChanged(qreal height);

public slots:
    void setTile(const Tile_ptr &tile);

private slots:
    void onXChanged();
    void onYChanged();
    void onWidthChanged();
    void onHeightChanged();

private:
    Q_DISABLE_COPY(Cell)

    const int m_index;
    QQuickItem *const m_cellItem;
    std::weak_ptr<Tile> m_tile;
};

} // namespace Internal
} // namespace Game

#endif // CELL_H
