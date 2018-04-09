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


#include "cell.h"
#include "game.h"
#include "gameboard.h"
#include "tile.h"

#include <QDebug>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QScreen>
#include <QSettings>

#include <algorithm>
#include <random>

static const char *const MAIN_WINDOW_FILE_PATH = "qrc:/qml/MainWindow.qml";
static const char *const TILE_FILE_PATH = "qrc:/qml/Tile.qml";

static const char *const GAME_OBJECT_NAME = "Game";
static const char *const GAMEBOARD_OBJECT_NAME = "Gameboard";

static const char *const PLAY_GAME_STATE_NAME = "play";
static const char *const WIN_GAME_STATE_NAME = "win";
static const char *const DEFEAT_GAME_STATE_NAME = "defeat";
static const char *const CONTINUE_GAME_STATE_NAME = "continue";

static const char *const GAME_STATE_PROPERTY_NAME = "state";
static const char *const SCORE_PROPERTY_NAME = "score";
static const char *const BEST_SCORE_PROPERTY_NAME = "bestScore";

static const char *const ADD_SCORE_FUNCTION_NAME = "addScore";

static const char *const WINDOW_X_SETTING_KEY_NAME = "x";
static const char *const WINDOW_Y_SETTING_KEY_NAME = "y";
static const char *const WINDOW_WIDTH_SETTING_KEY_NAME = "width";
static const char *const WINDOW_HEIGHT_SETTING_KEY_NAME = "height";
static const char *const BEST_SCORE_SETTING_KEY_NAME = "bestScore";

static const int DEFAULT_GAMEBOARD_WIDTH = 4;
static const int DEFAULT_GAMEBOARD_HEIGHT = 4;
static const int START_TILES_COUNT = 2;

static const int WINNING_VALUE = 2048;


class GamePrivate final
{
public:
    enum GameState : quint16 {
        Play,
        Win,
        Defeat,
        Continue
    };

    enum MoveDirection : quint8 {
      MoveLeft,
      MoveRight,
      MoveUp,
      MoveDown
    };

    explicit GamePrivate(Game *parent);
    ~GamePrivate();

    void updateCells();
    void setGameboardSize(int rows, int columns);
    void clearTiles();
    void setGameState(GameState state);
    void startGame();
    void startGame(int rows, int columns);
    void continueGame();
    void createStartTiles();
    void createTile(int cellIndex, int value);
    void createRandomTile();
    void setMoveKeysBlocked(bool blocked);
    void move(MoveDirection direction);
    void moveTile(const std::shared_ptr<Cell> &sourceCell, const std::shared_ptr<Cell> &targetCell);
    void addScore(int value);
    void setScore(int value);
    void setBestScore(int value);
    int bestScore() const;
    void setOrphaned(const std::shared_ptr<Tile> &tile);
    bool isDefeat() const;
    void readSettings();
    void saveSettings();
    qreal random();


    Game *const q;

    const std::unique_ptr<QQmlApplicationEngine> m_qmlEngine;
    const std::unique_ptr<QQmlComponent> m_tileQmlComponent;
    const std::unique_ptr<QSettings> m_settings;

    QQuickWindow *m_windowQuickItem;
    QQuickItem *m_gameQuickItem;

    std::mt19937 m_randomEngine;
    std::uniform_real_distribution<> m_randomDistribution;

    std::unique_ptr<Gameboard> m_gameboard;
    QMap<int, std::shared_ptr<Cell>> m_cells;
    QList<std::shared_ptr<Tile>> m_tiles;
    QList<std::shared_ptr<Tile>> m_aboutToOrphanedTiles;
    QList<std::shared_ptr<Tile>> m_orphanedTiles;
    int m_movingTilesCount;
    bool m_isMoveKeysBlocked;
    bool m_won;
    GameState m_gameState;
};


GamePrivate::GamePrivate(Game *parent) :
    q(parent),
    m_qmlEngine(std::make_unique<QQmlApplicationEngine>(parent)),
    m_tileQmlComponent(std::make_unique<QQmlComponent>(m_qmlEngine.get(), QUrl(QLatin1Literal(TILE_FILE_PATH)))),
    m_settings(std::make_unique<QSettings>()),
    m_gameQuickItem(nullptr),
    m_randomEngine(std::random_device()()),
    m_randomDistribution(0.0, 1.0),
    m_movingTilesCount(0),
    m_isMoveKeysBlocked(false),
    m_won(false),
    m_gameState(Play)
{
}


GamePrivate::~GamePrivate()
{
    saveSettings();
}


void GamePrivate::updateCells()
{
    m_cells = m_gameboard->cells();
}


void GamePrivate::setGameboardSize(int rows, int columns)
{
    m_gameboard->setRows(rows);
    m_gameboard->setColumns(columns);
}


void GamePrivate::clearTiles()
{
    std::for_each(m_tiles.begin(), m_tiles.end(), std::bind(&GamePrivate::setOrphaned, this, std::placeholders::_1));
    m_tiles.clear();
}


void GamePrivate::setGameState(GamePrivate::GameState state)
{
    QString stateName;

    switch (state) {
    case Play:
        stateName = QLatin1Literal(PLAY_GAME_STATE_NAME);
        break;
    case Win:
        stateName = QLatin1Literal(WIN_GAME_STATE_NAME);
        break;
    case Defeat:
        stateName = QLatin1Literal(DEFEAT_GAME_STATE_NAME);
        break;
    case Continue:
        stateName = QLatin1Literal(CONTINUE_GAME_STATE_NAME);
        break;
    }

    m_gameQuickItem->setProperty(GAME_STATE_PROPERTY_NAME, stateName);
    m_gameState = state;
}


void GamePrivate::startGame()
{
    startGame(DEFAULT_GAMEBOARD_WIDTH, DEFAULT_GAMEBOARD_HEIGHT);
}


void GamePrivate::startGame(int rows, int columns)
{
    setGameState(Play);
    setGameboardSize(rows, columns);
    setScore(0);
    clearTiles();
    updateCells();
    createStartTiles();
    setMoveKeysBlocked(false);
}


void GamePrivate::continueGame()
{
    setGameState(GamePrivate::Continue);
    createRandomTile();
    setMoveKeysBlocked(false);
}


void GamePrivate::createStartTiles()
{
    for (int i = 0; i < START_TILES_COUNT; ++i) {
        createRandomTile();
    }
}


void GamePrivate::createTile(int cellIndex, int value)
{
    std::shared_ptr<Tile> tile;

    if (m_orphanedTiles.empty()) {
        tile = std::make_shared<Tile>(m_tileQmlComponent.get(), q_check_ptr(m_gameboard->tilesParent()));
        QObject::connect(tile.get(), &Tile::moveFinished, q, &Game::onTileMoveFinished);
    } else {
        tile = m_orphanedTiles.takeLast();
    }

    m_tiles.append(tile);

    auto cell = m_cells.value(cellIndex);
    cell->setTile(tile);
    tile->setValue(value);
}


void GamePrivate::createRandomTile()
{
    QList<int> indexes;

    for (int index : m_cells.keys()) {
        auto cell = m_cells.value(index);
        if (!cell->tile()) {
            indexes.append(index);
        }
    }

    const int index = indexes.at(int(std::floor(random() * (indexes.count() - 1))));
    const int value = random() < 0.9 ? 2 : 4;

    createTile(index, value);
}


void GamePrivate::setMoveKeysBlocked(bool blocked)
{
    m_isMoveKeysBlocked = blocked;
}


void GamePrivate::move(GamePrivate::MoveDirection direction)
{
    int rows = 0;
    int columns = 0;

    switch (direction) {
    case MoveLeft:
    case MoveRight:
        rows = m_gameboard->rows();
        columns = m_gameboard->columns();
        break;
    case MoveUp:
    case MoveDown:
        rows = m_gameboard->columns();
        columns = m_gameboard->rows();
        break;
    }

    for (int row = 0; row < rows; ++row) {
        int firstCellIndex = 0;
        int neighborCellsIndexDelta = 0;

        switch (direction) {
        case MoveLeft:
            firstCellIndex = row * columns;
            neighborCellsIndexDelta = 1;
            break;
        case MoveRight:
            firstCellIndex = row * columns + columns - 1;
            neighborCellsIndexDelta = 1;
            break;
        case MoveUp:
            firstCellIndex = row;
            neighborCellsIndexDelta = rows;
            break;
        case MoveDown:
            firstCellIndex = columns * (rows - 1) + row;
            neighborCellsIndexDelta = rows;
            break;
        }

        int previousCellindex = firstCellIndex;

        for (int column = 1; column < columns; ++column) {
            int cellIndex = 0;
            switch (direction) {
            case MoveLeft:
                cellIndex = firstCellIndex + column;
                break;
            case MoveRight:
                cellIndex = firstCellIndex - column;
                break;
            case MoveUp:
                cellIndex = column * rows + row;
                break;
            case MoveDown:
                cellIndex = columns * (rows - 1) - column * rows + row;
                break;
            }

            auto cell = m_cells.value(cellIndex);

            if (!cell->tile()) {
                continue;
            }

            auto previousCell = m_cells.value(previousCellindex);
            if (!previousCell->tile()) {
                auto tile = cell->tile();
                tile->setZ(0);
                moveTile(cell, previousCell);
                continue;
            }

            if (previousCell->tile()->value() == cell->tile()->value()) {
                auto tile = cell->tile();
                tile->setValue(tile->value() * 2);
                tile->setZ(previousCell->tile()->z() + 1);
                m_aboutToOrphanedTiles.append(previousCell->tile());
                m_tiles.removeOne(previousCell->tile());
                moveTile(cell, previousCell);
                switch (direction) {
                case MoveLeft:
                    ++previousCellindex;
                    break;
                case MoveRight:
                    --previousCellindex;
                    break;
                case MoveUp:
                    previousCellindex += rows;
                    break;
                case MoveDown:
                    previousCellindex -= rows;
                    break;
                }
                continue;
            }

            int cellsIndexDelta = 0;
            switch (direction) {
            case MoveLeft:
            case MoveUp:
                cellsIndexDelta = cellIndex - previousCellindex;
                break;
            case MoveRight:
            case MoveDown:
                cellsIndexDelta = previousCellindex - cellIndex;
                break;
            }

            if (neighborCellsIndexDelta != cellsIndexDelta) {
                switch (direction) {
                case MoveLeft:
                case MoveUp:
                    previousCellindex += neighborCellsIndexDelta;
                    break;
                case MoveRight:
                case MoveDown:
                    previousCellindex -= neighborCellsIndexDelta;
                    break;
                }
                previousCell = m_cells.value(previousCellindex);
                cell->tile()->setZ(0);
                moveTile(cell, previousCell);
                continue;
            }

            previousCellindex = cellIndex;
        }
    }

    setMoveKeysBlocked(0 != m_movingTilesCount);
}


void GamePrivate::moveTile(const std::shared_ptr<Cell> &sourceCell, const std::shared_ptr<Cell> &targetCell)
{
    auto tile = sourceCell->tile();
    sourceCell->setTile(nullptr);
    targetCell->setTile(tile);
    ++m_movingTilesCount;
}


void GamePrivate::addScore(int value)
{
    if (0 < value) {
        QMetaObject::invokeMethod(m_gameQuickItem, ADD_SCORE_FUNCTION_NAME, Q_ARG(QVariant, value));
    }
}


void GamePrivate::setScore(int value)
{
    m_gameQuickItem->setProperty(SCORE_PROPERTY_NAME, value);
}


void GamePrivate::setBestScore(int value)
{
    m_gameQuickItem->setProperty(BEST_SCORE_PROPERTY_NAME, value);
}


int GamePrivate::bestScore() const
{
    return m_gameQuickItem->property(BEST_SCORE_PROPERTY_NAME).toInt();
}


void GamePrivate::setOrphaned(const std::shared_ptr<Tile> &tile)
{
    if (!m_orphanedTiles.contains(tile)) {
        tile->resetValue();
        m_orphanedTiles.append(tile);
    }
}


bool GamePrivate::isDefeat() const
{
    if (m_tiles.count() < m_cells.count()) {
        return false;
    }

    const int rows = m_gameboard->rows();
    const int columns = m_gameboard->columns();

    const int latestRow = rows - 1;
    const int latestColumn = columns - 1;

    std::shared_ptr<Cell> cell;

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            const int cellIndex = row * columns + column;
            cell = m_cells.value(cellIndex);
            Q_ASSERT(cell->tile());
            const int value = cell->tile()->value();

            if (column < latestColumn) {
                const int rightCellIndex = cellIndex + 1;
                cell = m_cells.value(rightCellIndex);
                Q_ASSERT(cell->tile());
                if (value == cell->tile()->value()) {
                    return false;
                }
            }

            if (row < latestRow) {
                const int bottomCellIndex = cellIndex + columns;
                cell = m_cells.value(bottomCellIndex);
                Q_ASSERT(cell->tile());
                if (value == cell->tile()->value()) {
                    return false;
                }
            }
        }
    }

    return true;
}


void GamePrivate::readSettings()
{
    Q_ASSERT(m_windowQuickItem);

    setBestScore(m_settings->value(QLatin1Literal(BEST_SCORE_SETTING_KEY_NAME)).toInt());

    if (!m_settings->contains(QLatin1Literal(WINDOW_X_SETTING_KEY_NAME))) {
        QRect geometry = m_windowQuickItem->geometry();
        const QPoint &centerOfScreen = QGuiApplication::primaryScreen()->availableGeometry().center();
        geometry.moveCenter(centerOfScreen);
        m_windowQuickItem->setGeometry(geometry);
        m_windowQuickItem->setVisibility(QWindow::AutomaticVisibility);
        return;
    }

    m_windowQuickItem->setX(m_settings->value(QLatin1Literal(WINDOW_X_SETTING_KEY_NAME)).toInt());
    m_windowQuickItem->setY(m_settings->value(QLatin1Literal(WINDOW_Y_SETTING_KEY_NAME)).toInt());
    m_windowQuickItem->setWidth(m_settings->value(QLatin1Literal(WINDOW_WIDTH_SETTING_KEY_NAME)).toInt());
    m_windowQuickItem->setHeight(m_settings->value(QLatin1Literal(WINDOW_HEIGHT_SETTING_KEY_NAME)).toInt());
    m_windowQuickItem->setVisibility(QWindow::AutomaticVisibility);
}


void GamePrivate::saveSettings()
{
    m_settings->setValue(QLatin1Literal(BEST_SCORE_SETTING_KEY_NAME), bestScore());
    m_settings->setValue(QLatin1Literal(WINDOW_X_SETTING_KEY_NAME), m_windowQuickItem->x());
    m_settings->setValue(QLatin1Literal(WINDOW_Y_SETTING_KEY_NAME), m_windowQuickItem->y());
    m_settings->setValue(QLatin1Literal(WINDOW_WIDTH_SETTING_KEY_NAME), m_windowQuickItem->width());
    m_settings->setValue(QLatin1Literal(WINDOW_HEIGHT_SETTING_KEY_NAME), m_windowQuickItem->height());
}


qreal GamePrivate::random()
{
    return m_randomDistribution(m_randomEngine);
}


Game::Game(QObject *parent) :
    QObject(parent),
    d(std::make_unique<GamePrivate>(this))
{
    connect(d->m_qmlEngine.get(), &QQmlApplicationEngine::objectCreated, this, &Game::onRootObjectCreated);
}


Game::~Game()
{
}


bool Game::launch()
{
    d->m_qmlEngine->load(QUrl(QLatin1Literal(MAIN_WINDOW_FILE_PATH)));
    if (d->m_qmlEngine->rootObjects().isEmpty()) {
        return false;
    }

    return true;
}


bool Game::eventFilter(QObject *object, QEvent *event)
{
    if (QEvent::KeyPress == event->type()) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (d->m_isMoveKeysBlocked) {
            if (Qt::Key_Enter == keyEvent->key() || Qt::Key_Return == keyEvent->key()) {
                switch (d->m_gameState) {
                case GamePrivate::Win:
                    d->continueGame();
                    return true;
                case GamePrivate::Defeat:
                    d->startGame();
                    return true;
                default:
                    break;
                }
            }
        } else {
            switch (keyEvent->key()) {
            case Qt::Key_Left:
                d->move(GamePrivate::MoveLeft);
                return true;
            case Qt::Key_Right:
                d->move(GamePrivate::MoveRight);
                return true;
            case Qt::Key_Up:
                d->move(GamePrivate::MoveUp);
                return true;
            case Qt::Key_Down:
                d->move(GamePrivate::MoveDown);
                return true;
            default:
                break;
            }
        }
    }

    return QObject::eventFilter(object, event);
}


void Game::onRootObjectCreated(QObject *object, const QUrl &url)
{
    Q_UNUSED(url)

    d->m_windowQuickItem = q_check_ptr(qobject_cast<QQuickWindow*>(object));
    d->m_windowQuickItem->installEventFilter(this);
    d->m_gameQuickItem = q_check_ptr(object->findChild<QQuickItem*>(QLatin1Literal(GAME_OBJECT_NAME)));
    connect(d->m_gameQuickItem, SIGNAL(continueGameRequested()), this, SLOT(onContinueGameRequested()));
    connect(d->m_gameQuickItem, SIGNAL(restartGameRequested()), this, SLOT(onRestartGameRequested()));

    QQuickItem *gameboardQuickItem = q_check_ptr(d->m_gameQuickItem->findChild<QQuickItem*>(QLatin1Literal(GAMEBOARD_OBJECT_NAME)));
    d->m_gameboard = std::make_unique<Gameboard>(gameboardQuickItem);

    d->readSettings();
    d->startGame();
}


void Game::onContinueGameRequested()
{
    d->continueGame();
}


void Game::onRestartGameRequested()
{
    d->startGame();
}


void Game::onTileMoveFinished()
{
    Q_ASSERT(d->m_movingTilesCount >= 0);

    Tile *tile = q_check_ptr(qobject_cast<Tile*>(sender()));

    if (WINNING_VALUE == tile->value() && GamePrivate::Continue != d->m_gameState) {
        d->m_won = true;
    }

    if (0 == --d->m_movingTilesCount) {
        int score = 0;
        for (const auto &tile : d->m_aboutToOrphanedTiles) {
            score += tile->value() * 2;
            d->setOrphaned(tile);
        }

        d->m_aboutToOrphanedTiles.clear();
        d->addScore(score);

        if (d->m_won) {
            d->setGameState(GamePrivate::Win);
            d->m_won = false;
            return;
        }

        d->createRandomTile();

        if (d->isDefeat()) {
            d->setGameState(GamePrivate::Defeat);
        } else {
            d->setMoveKeysBlocked(false);
        }
    }
}
