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


#ifndef GAME_H
#define GAME_H

#include <QObject>

#include <memory>

class GamePrivate;


class Game final : public QObject
{
    Q_OBJECT
public:
    explicit Game(QObject *parent = nullptr);
    ~Game();

    bool launch();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void onRootObjectCreated(QObject *object, const QUrl &url);
    void onContinueGameRequested();
    void onRestartGameRequested();
    void onTileMoveAnimationFinished();

private:
    Q_DISABLE_COPY(Game)

    const std::unique_ptr<GamePrivate> d;

    friend class GamePrivate;
};

#endif // GAME_H
