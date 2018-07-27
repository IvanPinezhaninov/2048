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

#include "game.h"
#include "gameboard.h"

#include <QDebug>
#include <QKeyEvent>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>


static const char *const MAIN_WINDOW_FILE_PATH = "qrc:/qml/MainWindow.qml";

static const char *const GAME_OBJECT_NAME = "Game";
static const char *const GAMEBOARD_OBJECT_NAME = "Gameboard";

static const char *const GAME_STATE_PROPERTY_NAME = "state";
static const char *const SCORE_PROPERTY_NAME = "score";
static const char *const BEST_SCORE_PROPERTY_NAME = "bestScore";
static const char *const UNDO_BUTTON_ENABLED_PROPERTY_NAME = "undoButtonEnabled";
static const char *const UNDO_BUTTON_ANIMATION_PROPERTY_NAME = "undoButtonAnimation";

static const char *const INIT_GAME_STATE_NAME = "init";
static const char *const PLAY_GAME_STATE_NAME = "play";
static const char *const WIN_GAME_STATE_NAME = "win";
static const char *const DEFEAT_GAME_STATE_NAME = "defeat";
static const char *const CONTINUE_GAME_STATE_NAME = "continue";


namespace Game {
namespace Internal {

class GamePrivate final
{
public:
    GamePrivate(Game *parent, QQmlApplicationEngine *qmlEngine);

    void processEnterKeyPressed();

    Game *const q;
    QQmlApplicationEngine *const m_qmlEngine;
    QQuickWindow *m_windowItem;
    QQuickItem *m_gameItem;
    std::unique_ptr<Gameboard> m_gameboard;
    int m_score;
    int m_bestScore;
    GameState m_gameState;
    bool m_ready;
};


GamePrivate::GamePrivate(Game *parent, QQmlApplicationEngine *qmlEngine) :
    q(parent),
    m_qmlEngine(qmlEngine),
    m_windowItem(nullptr),
    m_gameItem(nullptr),
    m_score(0),
    m_bestScore(0),
    m_gameState(GameState::Init),
    m_ready(false)
{
}


void GamePrivate::processEnterKeyPressed()
{
    switch (m_gameState) {
    case GameState::Win:
        emit q->continueGameRequested();
        break;
    case GameState::Defeat:
        emit q->startNewGameRequested();
        break;
    default:
        break;
    }
}


Game::Game(QQmlApplicationEngine *qmlEngine, QObject *parent) :
    QObject(parent),
    d(std::make_unique<GamePrivate>(this, qmlEngine))
{
    connect(this, &Game::scoreChanged, this, &Game::onScoreChanged);
    connect(this, &Game::bestScoreChanged, this, &Game::onBestScoreChanged);
    connect(d->m_qmlEngine, &QQmlApplicationEngine::objectCreated, this, &Game::onRootObjectCreated);
}


Game::~Game()
{
}


bool Game::init()
{
    d->m_qmlEngine->load(QUrl(QLatin1Literal(MAIN_WINDOW_FILE_PATH)));
    if (d->m_qmlEngine->rootObjects().isEmpty()) {
        qCritical() << "Can't find QML root objects";
        return false;
    }

    return true;
}


bool Game::isReady() const
{
    return d->m_ready;
}


void Game::setGeometry(const QRect &rect)
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->setGeometry(rect);
}


void Game::setGeometry(int x, int y, int w, int h)
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->setGeometry(x, y, w, h);
}


QRect Game::geometry() const
{
    Q_ASSERT(nullptr != d->m_windowItem);

    return d->m_windowItem->geometry();
}


bool Game::isVisible() const
{
    Q_ASSERT(nullptr != d->m_windowItem);

    return d->m_windowItem->isVisible();
}


Qt::WindowState Game::windowState() const
{
    Q_ASSERT(nullptr != d->m_windowItem);

    return d->m_windowItem->windowState();
}


#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))

Qt::WindowStates Game::windowStates() const
{
    Q_ASSERT(nullptr != d->m_windowItem);

    return d->m_windowItem->windowStates();
}

#endif


int Game::score() const
{
    return d->m_score;
}


int Game::bestScore() const
{
    return d->m_bestScore;
}


QList<Cell_ptr> Game::cells() const
{
    return d->m_gameboard->cells();
}


GameState Game::gameState() const
{
    return d->m_gameState;
}


int Game::gameboardRows() const
{
    return d->m_gameboard->rows();
}


int Game::gameboardColumns() const
{
    return d->m_gameboard->columns();
}


QQuickItem *Game::tilesParent() const
{
    return d->m_gameboard->tilesParent();
}


void Game::setUndoButtonEnabled(bool enabled, bool animation)
{
    d->m_gameItem->setProperty(UNDO_BUTTON_ANIMATION_PROPERTY_NAME, animation);
    d->m_gameItem->setProperty(UNDO_BUTTON_ENABLED_PROPERTY_NAME, enabled);
}


bool Game::isUndoButtonEnabled() const
{
    return d->m_gameItem->property(UNDO_BUTTON_ENABLED_PROPERTY_NAME).toBool();
}


void Game::showFullScreen()
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->showFullScreen();
}


void Game::showMaximized()
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->showMaximized();
}


void Game::showMinimized()
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->showMinimized();
}


void Game::showNormal()
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->showNormal();
}


void Game::show()
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->show();
}

void Game::setVisible(bool visible)
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->setVisible(visible);
}


void Game::setWindowState(Qt::WindowState state)
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->setWindowState(state);
}



#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))

void Game::setWindowStates(Qt::WindowStates states)
{
    Q_ASSERT(nullptr != d->m_windowItem);

    d->m_windowItem->setWindowStates(states);
}

#endif



void Game::setScore(int score)
{
    if (d->m_score != score) {
        d->m_score = score;
        emit scoreChanged(score);
    }
}


void Game::setBestScore(int score)
{
    if (d->m_bestScore != score) {
        d->m_bestScore = score;
        emit bestScoreChanged(score);
    }
}


void Game::setGameboardRows(int rows)
{
    d->m_gameboard->setRows(rows);
}


void Game::setGameboardColumns(int columns)
{
    d->m_gameboard->setColumns(columns);
}


void Game::setGameboardSize(int rows, int columns)
{
    d->m_gameboard->setSize(rows, columns);
}


void Game::setGameState(GameState state)
{
    if (d->m_gameState != state) {
        d->m_gameState = state;

        QString stateName;

        switch (state) {
        case GameState::Init:
            stateName = QLatin1Literal(INIT_GAME_STATE_NAME);
            break;
        case GameState::Play:
            stateName = QLatin1Literal(PLAY_GAME_STATE_NAME);
            break;
        case GameState::Win:
            stateName = QLatin1Literal(WIN_GAME_STATE_NAME);
            break;
        case GameState::Defeat:
            stateName = QLatin1Literal(DEFEAT_GAME_STATE_NAME);
            break;
        case GameState::Continue:
            stateName = QLatin1Literal(CONTINUE_GAME_STATE_NAME);
            break;
        }

        d->m_gameItem->setProperty(GAME_STATE_PROPERTY_NAME, stateName);

        emit gameStateChanged(state);
    }
}


bool Game::eventFilter(QObject *object, QEvent *event)
{
    if (QEvent::KeyPress == event->type()) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        switch (keyEvent->key()) {
        case Qt::Key_Left:
            emit moveTilesRequested(MoveDirection::Left);
            break;
        case Qt::Key_Right:
            emit moveTilesRequested(MoveDirection::Right);
            break;
        case Qt::Key_Up:
            emit moveTilesRequested(MoveDirection::Up);
            break;
        case Qt::Key_Down:
            emit moveTilesRequested(MoveDirection::Down);
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            d->processEnterKeyPressed();
            break;
        case Qt::Key_Backspace:
            emit undoRequested();
            break;
        default:
            break;
        }
    }

    return QObject::eventFilter(object, event);
}


void Game::onRootObjectCreated(QObject *object, const QUrl &url)
{
    Q_UNUSED(url)

    d->m_windowItem = q_check_ptr(qobject_cast<QQuickWindow*>(object));
    d->m_windowItem->installEventFilter(this);

    d->m_gameItem = q_check_ptr(object->findChild<QQuickItem*>(QLatin1Literal(GAME_OBJECT_NAME)));
    connect(d->m_gameItem, SIGNAL(continueGameRequested()), this, SIGNAL(continueGameRequested()));
    connect(d->m_gameItem, SIGNAL(startNewGameRequested()), this, SIGNAL(startNewGameRequested()));
    connect(d->m_gameItem, SIGNAL(undoRequested()), this, SIGNAL(undoRequested()));

    QQuickItem *gameboardItem = q_check_ptr(d->m_gameItem->findChild<QQuickItem*>(QLatin1Literal(GAMEBOARD_OBJECT_NAME)));
    d->m_gameboard = std::make_unique<Gameboard>(gameboardItem);
    connect(d->m_gameboard.get(), &Gameboard::sizeChanged, this, &Game::gameboardSizeChanged);
    connect(d->m_gameboard.get(), &Gameboard::rowsChanged, this, &Game::gameboardRowsChanged);
    connect(d->m_gameboard.get(), &Gameboard::columnsChanged, this, &Game::gameboardColumnsChanged);
    connect(d->m_gameboard.get(), &Gameboard::cellsChanged, this, &Game::cellsChanged);

    d->m_ready = true;
    emit gameReady();
}


void Game::onScoreChanged(int score)
{
    Q_ASSERT(nullptr != d->m_gameItem);

    d->m_gameItem->setProperty(SCORE_PROPERTY_NAME, score);

    if (d->m_bestScore < score) {
        setBestScore(score);
    }
}


void Game::onBestScoreChanged(int score)
{
    Q_ASSERT(nullptr != d->m_gameItem);

    d->m_gameItem->setProperty(BEST_SCORE_PROPERTY_NAME, score);
}

} // namespace Internal
} // namespace Game
