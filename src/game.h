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

QT_BEGIN_NAMESPACE
class QQmlApplicationEngine;
class QQuickItem;
QT_END_NAMESPACE

#include <memory>

#include "movedirection.h"


namespace Game {
namespace Internal {

class Cell;
class GamePrivate;
class GameboardSize;

using Cell_ptr = std::shared_ptr<Cell>;


class Game final : public QObject
{
    Q_OBJECT
public:
    enum class GameState : quint8
    {
        Play,
        Win,
        Defeat,
        Continue
    };

    explicit Game(QQmlApplicationEngine *qmlEngine, QObject *parent = nullptr);
    ~Game();

    bool init();

    void setGeometry(const QRect &rect);
    void setGeometry(int x, int y, int w, int h);
    QRect geometry() const;

    bool isVisible() const;
    Qt::WindowState windowState() const;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    Qt::WindowStates windowStates() const;
#endif

    int score() const;
    int bestScore() const;
    GameboardSize gameboardSize() const;
    GameState gameState() const;

    QList<Cell_ptr> cells() const;
    QQuickItem *tilesParent() const;

signals:
    void gameReady();
    void scoreChanged(int score);
    void bestScoreChanged(int score);
    void gameboardSizeChanged(const GameboardSize &size);
    void gameStateChanged(GameState state);
    void cellsChanged(const QList<Cell_ptr> &cells);
    void startNewGameRequested();
    void continueGameRequested();
    void moveTilesRequested(MoveDirection moveDirection);

public slots:
    void showFullScreen();
    void showMaximized();
    void showMinimized();
    void showNormal();
    void show();

    void setVisible(bool visible);
    void setWindowState(Qt::WindowState state);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    void setWindowStates(Qt::WindowStates states);
#endif

    void setScore(int score);
    void setBestScore(int score);
    void setGameboardSize(const GameboardSize &size);
    void setGameState(GameState state);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void onRootObjectCreated(QObject *object, const QUrl &url);
    void onScoreChanged(int score);
    void onBestScoreChanged(int score);

private:
    Q_DISABLE_COPY(Game)

    const std::unique_ptr<GamePrivate> d;

    friend class GamePrivate;
};

} // namespace Internal
} // namespace Game

#endif // GAME_H
