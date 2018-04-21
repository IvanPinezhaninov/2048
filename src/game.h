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


namespace Game {
namespace Internal {

class GamePrivate;

class Game final : public QObject
{
    Q_OBJECT
public:
    enum MoveDirection : quint8 {
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown
    };

    explicit Game(QObject *parent = nullptr);
    ~Game();

    bool init();

    void setGeometry(const QRect &rect);
    void setGeometry(int x, int y, int w, int h);
    QRect geometry() const;

    bool isVisible() const;
    Qt::WindowState windowState() const;
    Qt::WindowStates windowStates() const;

    void setScore(int score);
    int score() const;

    void setBestScore(int score);
    int bestScore() const;

public slots:
    void showFullScreen();
    void showMaximized();
    void showMinimized();
    void showNormal();
    void show();

    void setVisible(bool visible);
    void setWindowState(Qt::WindowState state);
    void setWindowStates(Qt::WindowStates states);

    void startNewGame();
    void startNewGame(int rows, int columns);
    void moveTiles(MoveDirection direction);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void onRootObjectCreated(QObject *object, const QUrl &url);
    void onContinueGameRequested();
    void onRestartGameRequested();
    void onTileMoveFinished();

private:
    Q_DISABLE_COPY(Game)

    const std::unique_ptr<GamePrivate> d;

    friend class GamePrivate;
};

} // namespace Internal
} // namespace Game

#endif // GAME_H
