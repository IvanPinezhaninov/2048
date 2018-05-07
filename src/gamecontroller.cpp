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
#include "gamecontroller.h"
#include "storage.h"
#include "tile.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QScreen>
#include <QSettings>
#include <QTimer>

#include <algorithm>
#include <cmath>
#include <random>

static const int SHOW_START_TILES_DELAY = 400;

static const char *const GAME_WINDOW_X_SETTING_KEY_NAME = "x";
static const char *const GAME_WINDOW_Y_SETTING_KEY_NAME = "y";
static const char *const GAME_WINDOW_WIDTH_SETTING_KEY_NAME = "width";
static const char *const GAME_WINDOW_HEIGHT_SETTING_KEY_NAME = "height";
#ifdef Q_OS_MACOS
static const char *const SETTINGS_FILE_LOCATION = "%1/../Resources/settings.ini";
#endif
static const char *const TILE_FILE_PATH = "qrc:/qml/Tile.qml";
static const int DEFAULT_GAMEBOARD_ROWS = 4;
static const int DEFAULT_GAMEBOARD_COLUMNS = 4;
static const int START_TILES_COUNT = 2;
static const int WINNING_VALUE = 2048;


namespace Game {

using GameState = Internal::GameState;
using MoveDirection = Internal::MoveDirection;
using StorageState = Internal::Storage::StorageState;

namespace Internal {

class GameControllerPrivate final
{
public:
    explicit GameControllerPrivate(GameController *parent);

    qreal random();
    int nextTileId();
    bool useStartTilesAnimation() const;
    void createTile(int cellIndex, int value);
    void createTile(int id, int cellIndex, int value, bool animation);
    void createRandomTile();
    void createStartTiles();
    void setOrphanedTile(const Tile_ptr &tile);
    void moveTiles();
    void moveTile(const Cell_ptr &sourceCell, const Cell_ptr &targetCell);
    void clearTiles();
    bool isDefeat() const;
    void setMoveBlocked(bool blocked);
    void setGameboardSize(int rows, int columns);
    void createNewGame(int rows, int columns);
    void saveTurn();

    void readSettings();
    void saveSettings();

    GameController *const q;
    const std::unique_ptr<QQmlApplicationEngine> m_qmlEngine;
    const std::unique_ptr<QQmlComponent> m_tileQmlComponent;
    const std::unique_ptr<Game> m_game;
    const std::unique_ptr<Storage> m_storage;
    const std::unique_ptr<QSettings> m_settings;
    std::mt19937 m_randomEngine;
    std::uniform_real_distribution<> m_randomDistribution;
    QList<Cell_ptr> m_cells;
    QList<Tile_ptr> m_tiles;
    QList<Tile_ptr> m_aboutToOrphanedTiles;
    QList<Tile_ptr> m_orphanedTiles;
    QList<TileSpec> m_restoredTiles;
    int m_tileId;
    int m_movingTilesCount;
    MoveDirection m_moveDirection;
    bool m_moveBlocked;
};


GameControllerPrivate::GameControllerPrivate(GameController *parent) :
    q(parent),
    m_qmlEngine(std::make_unique<QQmlApplicationEngine>(parent)),
    m_tileQmlComponent(std::make_unique<QQmlComponent>(m_qmlEngine.get(), QUrl(QLatin1Literal(TILE_FILE_PATH)))),
    m_game(std::make_unique<Game>(m_qmlEngine.get(), parent)),
    m_storage(std::make_unique<Storage>(parent)),
#ifdef Q_OS_MACOS
    m_settings(std::make_unique<QSettings>(QString(QLatin1Literal(SETTINGS_FILE_LOCATION))
                                           .arg(QCoreApplication::applicationDirPath()), QSettings::IniFormat)),
#else
    m_settings(std::make_unique<QSettings>()),
#endif
  m_randomEngine(std::random_device()()),
  m_randomDistribution(0.0, 1.0),
  m_tileId(0),
  m_movingTilesCount(0),
  m_moveDirection(MoveDirection::None),
  m_moveBlocked(true)
{
}


qreal GameControllerPrivate::random()
{
    return m_randomDistribution(m_randomEngine);
}


int GameControllerPrivate::nextTileId()
{
    return ++m_tileId;
}


bool GameControllerPrivate::useStartTilesAnimation() const
{
    return (START_TILES_COUNT == m_restoredTiles.size() && 0 == m_game->score());
}


void GameControllerPrivate::createTile(int cellIndex, int value)
{
    const int id = nextTileId();
    createTile(id, cellIndex, value, true);
}


void GameControllerPrivate::createTile(int id, int cellIndex, int value, bool animation)
{
    Tile_ptr tile;

    if (m_orphanedTiles.empty()) {
        tile = std::make_shared<Tile>(id, m_tileQmlComponent.get(), q_check_ptr(m_game->tilesParent()), animation);
        QObject::connect(tile.get(), &Tile::moveFinished, q, &GameController::onTileMoveFinished);
    } else {
        tile = m_orphanedTiles.takeLast();
        tile->setId(id);
    }

    m_tiles.append(tile);

    const auto &cell = m_cells.at(cellIndex);
    cell->setTile(tile);
    tile->setValue(value);
}


void GameControllerPrivate::createRandomTile()
{
    QList<int> indexes;
    int cellsCount = m_cells.size();

    for (int i = 0; i < cellsCount; ++i) {
        const auto &cell = m_cells.at(i);
        if (!cell->tile()) {
            indexes.append(i);
        }
    }

    const int index = indexes.at(int(std::floor(random() * (indexes.size() - 1))));
    const int value = random() < 0.9 ? 2 : 4;

    createTile(index, value);
}


void GameControllerPrivate::createStartTiles()
{
    for (int i = 0; i < START_TILES_COUNT; ++i) {
        createRandomTile();
    }
}


void GameControllerPrivate::setOrphanedTile(const Tile_ptr &tile)
{
    if (!m_orphanedTiles.contains(tile)) {
        tile->resetValue();
        m_orphanedTiles.append(tile);
    }
}


void GameControllerPrivate::moveTiles()
{
    Q_ASSERT(MoveDirection::None != m_moveDirection);

    if (m_moveBlocked) {
        return;
    }

    int rows = 0;
    int columns = 0;

    switch (m_moveDirection) {
    case MoveDirection::Left:
    case MoveDirection::Right:
        rows = m_game->gameboardRows();
        columns = m_game->gameboardColumns();
        break;
    case MoveDirection::Up:
    case MoveDirection::Down:
        rows = m_game->gameboardColumns();
        columns = m_game->gameboardRows();
        break;
    case MoveDirection::None:
        Q_ASSERT(false);
        return;
    }

    for (int row = 0; row < rows; ++row) {
        int firstCellIndex = 0;
        int neighborCellsIndexDelta = 0;

        switch (m_moveDirection) {
        case MoveDirection::Left:
            firstCellIndex = row * columns;
            neighborCellsIndexDelta = 1;
            break;
        case MoveDirection::Right:
            firstCellIndex = row * columns + columns - 1;
            neighborCellsIndexDelta = 1;
            break;
        case MoveDirection::Up:
            firstCellIndex = row;
            neighborCellsIndexDelta = rows;
            break;
        case MoveDirection::Down:
            firstCellIndex = columns * (rows - 1) + row;
            neighborCellsIndexDelta = rows;
            break;
        case MoveDirection::None:
            Q_ASSERT(false);
            break;
        }

        int previousCellIndex = firstCellIndex;

        for (int column = 1; column < columns; ++column) {
            int cellIndex = 0;
            switch (m_moveDirection) {
            case MoveDirection::Left:
                cellIndex = firstCellIndex + column;
                break;
            case MoveDirection::Right:
                cellIndex = firstCellIndex - column;
                break;
            case MoveDirection::Up:
                cellIndex = column * rows + row;
                break;
            case MoveDirection::Down:
                cellIndex = columns * (rows - 1) - column * rows + row;
                break;
            case MoveDirection::None:
                Q_ASSERT(false);
                break;
            }

            const auto &cell = m_cells.at(cellIndex);

            if (!cell->tile()) {
                continue;
            }

            auto previousCell = m_cells.at(previousCellIndex);
            if (!previousCell->tile()) {
                const auto &tile = cell->tile();
                tile->setZ(0);
                moveTile(cell, previousCell);
                continue;
            }

            if (previousCell->tile()->value() == cell->tile()->value()) {
                const auto &tile = cell->tile();
                tile->setValue(tile->value() * 2);
                tile->setZ(previousCell->tile()->z() + 1);
                m_aboutToOrphanedTiles.append(previousCell->tile());
                m_tiles.removeOne(previousCell->tile());
                moveTile(cell, previousCell);
                switch (m_moveDirection) {
                case MoveDirection::Left:
                    ++previousCellIndex;
                    break;
                case MoveDirection::Right:
                    --previousCellIndex;
                    break;
                case MoveDirection::Up:
                    previousCellIndex += rows;
                    break;
                case MoveDirection::Down:
                    previousCellIndex -= rows;
                    break;
                case MoveDirection::None:
                    Q_ASSERT(false);
                    break;
                }
                continue;
            }

            int cellsIndexDelta = 0;
            switch (m_moveDirection) {
            case MoveDirection::Left:
            case MoveDirection::Up:
                cellsIndexDelta = cellIndex - previousCellIndex;
                break;
            case MoveDirection::Right:
            case MoveDirection::Down:
                cellsIndexDelta = previousCellIndex - cellIndex;
                break;
            case MoveDirection::None:
                Q_ASSERT(false);
                break;
            }

            if (neighborCellsIndexDelta != cellsIndexDelta) {
                switch (m_moveDirection) {
                case MoveDirection::Left:
                case MoveDirection::Up:
                    previousCellIndex += neighborCellsIndexDelta;
                    break;
                case MoveDirection::Right:
                case MoveDirection::Down:
                    previousCellIndex -= neighborCellsIndexDelta;
                    break;
                case MoveDirection::None:
                    Q_ASSERT(false);
                    break;
                }
                previousCell = m_cells.at(previousCellIndex);
                cell->tile()->setZ(0);
                moveTile(cell, previousCell);
                continue;
            }

            previousCellIndex = cellIndex;
        }
    }

    setMoveBlocked(0 != m_movingTilesCount);
}


void GameControllerPrivate::moveTile(const Cell_ptr &sourceCell, const Cell_ptr &targetCell)
{
    const auto &tile = sourceCell->tile();
    sourceCell->setTile(nullptr);
    targetCell->setTile(tile);
    ++m_movingTilesCount;
}


void GameControllerPrivate::clearTiles()
{
    for (const auto &tile : m_tiles) {
        tile->resetValue();
        tile->setCell(nullptr);
        m_orphanedTiles.append(tile);
    }
    m_tiles.clear();
}


bool GameControllerPrivate::isDefeat() const
{
    if (m_tiles.size() < m_cells.size()) {
        return false;
    }

    const int rows = m_game->gameboardRows();
    const int columns = m_game->gameboardColumns();

    const int latestRow = rows - 1;
    const int latestColumn = columns - 1;

    Cell_ptr cell;

    for (int row = 0; row < rows; ++row) {
        for (int column = 0; column < columns; ++column) {
            const int cellIndex = row * columns + column;
            cell = m_cells.at(cellIndex);
            Q_ASSERT(cell->tile());
            const int value = cell->tile()->value();

            if (column < latestColumn) {
                const int rightCellIndex = cellIndex + 1;
                cell = m_cells.at(rightCellIndex);
                Q_ASSERT(cell->tile());
                if (value == cell->tile()->value()) {
                    return false;
                }
            }

            if (row < latestRow) {
                const int bottomCellIndex = cellIndex + columns;
                cell = m_cells.at(bottomCellIndex);
                Q_ASSERT(cell->tile());
                if (value == cell->tile()->value()) {
                    return false;
                }
            }
        }
    }

    return true;
}


void GameControllerPrivate::setMoveBlocked(bool blocked)
{
    m_moveBlocked = blocked;
}


void GameControllerPrivate::setGameboardSize(int rows, int columns)
{
    m_game->setGameboardSize(rows, columns);
    m_cells = m_game->cells();
}


void GameControllerPrivate::createNewGame(int rows, int columns)
{
    setGameboardSize(rows, columns);

    switch (m_storage->state()) {
    case StorageState::Ready:
        m_storage->createGame(rows, columns);
        break;
    case StorageState::Error:
        q->startNewGame();
        break;
    case StorageState::NotReady:
        Q_ASSERT(false);
        break;
    }
}


void GameControllerPrivate::saveTurn()
{
    QList<TileSpec> tiles;
    for (const auto &tile : m_tiles) {
        tiles.append({ tile->id(), tile->cell()->index(), tile->value() });
    }

    m_storage->saveTurn(m_moveDirection, tiles, m_game->score(), m_game->bestScore());
}


void GameControllerPrivate::readSettings()
{
    if (!m_settings->contains(QLatin1Literal(GAME_WINDOW_X_SETTING_KEY_NAME))) {
        // Move game window to center of screen
        QRect rect = m_game->geometry();
        const QPoint &centerOfScreen = QGuiApplication::primaryScreen()->availableGeometry().center();
        rect.moveCenter(centerOfScreen);
        m_game->setGeometry(rect);
        return;
    }

    // Game window geometry
    const int x = m_settings->value(QLatin1Literal(GAME_WINDOW_X_SETTING_KEY_NAME)).toInt();
    const int y = m_settings->value(QLatin1Literal(GAME_WINDOW_Y_SETTING_KEY_NAME)).toInt();
    const int w = m_settings->value(QLatin1Literal(GAME_WINDOW_WIDTH_SETTING_KEY_NAME)).toInt();
    const int h = m_settings->value(QLatin1Literal(GAME_WINDOW_HEIGHT_SETTING_KEY_NAME)).toInt();
    m_game->setGeometry(x, y, w, h);
}


void GameControllerPrivate::saveSettings()
{
    // Game window geometry
    const QRect &rect = m_game->geometry();
    m_settings->setValue(QLatin1Literal(GAME_WINDOW_X_SETTING_KEY_NAME), rect.x());
    m_settings->setValue(QLatin1Literal(GAME_WINDOW_Y_SETTING_KEY_NAME), rect.y());
    m_settings->setValue(QLatin1Literal(GAME_WINDOW_WIDTH_SETTING_KEY_NAME), rect.width());
    m_settings->setValue(QLatin1Literal(GAME_WINDOW_HEIGHT_SETTING_KEY_NAME), rect.height());
}

} // namespace Internal


GameController::GameController(QObject *parent) :
    QObject(parent),
    d(std::make_unique<Internal::GameControllerPrivate>(this))
{
    connect(d->m_game.get(), &Game::Internal::Game::gameReady, this, &GameController::onGameReady);
    connect(d->m_game.get(), &Game::Internal::Game::moveTilesRequested, this, &GameController::onMoveTilesRequested);
    connect(d->m_game.get(), &Game::Internal::Game::startNewGameRequested, this, &GameController::onStartNewGameRequested);
    connect(d->m_game.get(), &Game::Internal::Game::continueGameRequested, this, &GameController::onContinueGameRequested);
    connect(d->m_storage.get(), &Game::Internal::Storage::storageReady, this, &GameController::onStorageReady);
    connect(d->m_storage.get(), &Game::Internal::Storage::storageError, this, &GameController::onStorageError);
    connect(d->m_storage.get(), &Game::Internal::Storage::gameCreated, this, &GameController::onGameCreated);
    connect(d->m_storage.get(), &Game::Internal::Storage::createGameError, this, &GameController::onCreateGameError);
    connect(d->m_storage.get(), &Game::Internal::Storage::gameRestored, this, &GameController::onGameRestored);
    connect(d->m_storage.get(), &Game::Internal::Storage::restoreGameError, this, &GameController::onRestoreGameError);
}


GameController::~GameController()
{
}


bool GameController::init()
{
    d->m_storage->init();
    return d->m_game->init();
}


void GameController::shutdown()
{
    d->saveSettings();
}


void GameController::onGameReady()
{
    d->readSettings();

    switch (d->m_storage->state()) {
    case StorageState::Ready:
        d->m_storage->restoreGame();
        break;
    case StorageState::Error:
        d->createNewGame(DEFAULT_GAMEBOARD_ROWS, DEFAULT_GAMEBOARD_COLUMNS);
        break;
    case StorageState::NotReady:
        // Waiting for the storage to be ready
        break;
    }
}


void GameController::onStartNewGameRequested()
{
    d->createNewGame(d->m_game->gameboardRows(), d->m_game->gameboardColumns());
}


void GameController::onContinueGameRequested()
{
    d->m_game->setGameState(GameState::Continue);
    d->createRandomTile();

    if (StorageState::Ready == d->m_storage->state()) {
        d->saveTurn();
    }

    d->setMoveBlocked(false);
}


void GameController::onMoveTilesRequested(Internal::MoveDirection direction)
{
    Q_ASSERT(Internal::MoveDirection::None != direction);

    d->m_moveDirection = direction;
    d->moveTiles();
}


void GameController::onTileMoveFinished()
{
    Q_ASSERT(d->m_movingTilesCount >= 0);

    using Tile = Internal::Tile;
    static bool win = false;

    Tile *tile = q_check_ptr(qobject_cast<Tile*>(sender()));

    if (WINNING_VALUE == tile->value() && GameState::Continue != d->m_game->gameState()) {
        win = true;
    }

    if (0 == --d->m_movingTilesCount) {
        int score = d->m_game->score();
        for (const auto &tile : d->m_aboutToOrphanedTiles) {
            score += tile->value() * 2;
            d->setOrphanedTile(tile);
        }

        d->m_aboutToOrphanedTiles.clear();
        d->m_game->setScore(score);

        if (win) {
            d->m_game->setGameState(GameState::Win);
            win = false;
        } else {
            d->createRandomTile();
        }

        bool moveBlocked = true;

        if (d->isDefeat()) {
            d->m_game->setGameState(GameState::Defeat);
        } else {
            moveBlocked = false;
        }

        if (StorageState::Ready == d->m_storage->state()) {
            d->saveTurn();
        }

        if (!moveBlocked) {
            d->setMoveBlocked(false);
        }
    }
}


void GameController::onStorageReady()
{
    if (d->m_game->isReady()) {
        d->m_storage->restoreGame();
    }
}


void GameController::onStorageError()
{
    if (d->m_game->isReady()) {
        d->createNewGame(DEFAULT_GAMEBOARD_ROWS, DEFAULT_GAMEBOARD_COLUMNS);
    }
}


void GameController::onGameCreated()
{
    const int delay = (GameState::Init == d->m_game->gameState()) ? SHOW_START_TILES_DELAY : 0;
    QTimer::singleShot(delay, this, SLOT(startNewGame()));
}


void GameController::onCreateGameError()
{
    const int delay = (GameState::Init == d->m_game->gameState()) ? SHOW_START_TILES_DELAY : 0;
    QTimer::singleShot(delay, this, SLOT(startNewGame()));
}


void GameController::onGameRestored(const Internal::GameSpec &gameSpec)
{
    Q_ASSERT(0 < gameSpec.rows() && 0 < gameSpec.columns());
    Q_ASSERT(!gameSpec.tiles().isEmpty());
    Q_ASSERT(!d->m_game->isVisible());

    d->m_restoredTiles = gameSpec.tiles();
    d->setGameboardSize(gameSpec.rows(), gameSpec.columns());
    d->m_game->setScore(gameSpec.score());
    d->m_game->setBestScore(gameSpec.bestScore());
    d->m_game->show();

    const int timeout = d->useStartTilesAnimation() ? SHOW_START_TILES_DELAY : 0;
    QTimer::singleShot(timeout, this, SLOT(restoreGame()));
}


void GameController::onRestoreGameError()
{
    Q_ASSERT(!d->m_game->isVisible());

    d->createNewGame(DEFAULT_GAMEBOARD_ROWS, DEFAULT_GAMEBOARD_COLUMNS);
    d->m_game->show();
}


void GameController::startNewGame()
{
    d->m_game->setScore(0);
    d->m_game->setGameState(GameState::Play);
    d->m_moveDirection = MoveDirection::None;
    d->clearTiles();
    d->createStartTiles();

    if (StorageState::Ready == d->m_storage->state()) {
        d->saveTurn();
    }

    d->setMoveBlocked(false);
}


void GameController::restoreGame()
{
    Q_ASSERT(GameState::Init == d->m_game->gameState());
    Q_ASSERT(!d->m_restoredTiles.isEmpty());

    GameState gameState = GameState::Play;
    const bool animation = d->useStartTilesAnimation();

    for (const auto &tile : d->m_restoredTiles) {
        d->createTile(tile.id(), tile.cell(), tile.value(), animation);
        if (WINNING_VALUE == tile.value()) {
            gameState = GameState::Continue;
        }
    }

    if (d->isDefeat()) {
        gameState = GameState::Defeat;
    }

    d->m_restoredTiles.clear();
    d->m_game->setGameState(gameState);
    d->setMoveBlocked(false);
}

} // namespace Game
