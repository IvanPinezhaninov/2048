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


class Tile final : public QObject
{
    Q_OBJECT
public:
    explicit Tile(QQmlComponent *tileQmlComponent, QQuickItem *parent);
    ~Tile();

    qreal x() const;
    qreal y() const;
    qreal z() const;
    qreal width() const;
    qreal height() const;

    int value() const;

  signals:
    void moveFinished();

public slots:
    void setValue(int value);
    void resetValue();

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
    int m_value;
};

#endif // TILE_H
