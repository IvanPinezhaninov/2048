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

#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickItem>
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

static const int FIRST_TURN_ID = 1;
static const int FIRST_PARENT_TURN_ID = 0;


namespace Game {

using Game = Internal::Game;
using GameState = Internal::GameState;
using Storage = Internal::Storage;
using StorageState = Internal::Storage::StorageState;
using Tile = Internal::Tile;
using Tile_ptr = Internal::Tile_ptr;

namespace Internal {

struct TileData
{
    int id;
    int value;
    int cell;
};

class GameControllerPrivate final
{
public:
    explicit GameControllerPrivate(GameController *parent);

    qreal random();
    int nextTileId();
    bool useRestoreAnimation() const;
    void createTile(int value, int cell);
    void createTile(int id, int value, int cell, bool animation);
    void createRandomTile();
    void createStartTiles();
    void hideTile(const Tile_ptr &tile, bool animation = true);
    void moveTile(const Cell_ptr &sourceCell, const Cell_ptr &targetCell);
    void clearTiles();
    bool isDefeat() const;
    void setGameboardSize(int rows, int columns);
    void createNewGame(int rows, int columns);
    void saveTurn();
    bool isFirstTurn() const;
    QVariantList tilesToList() const;
    QVariantMap tileToMap(const Tile_ptr &tile) const;
    TileData tileFromVariant(const QVariant &tile) const;
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
    QList<Tile_ptr> m_aboutToHiddenTiles;
    QList<Tile_ptr> m_hiddenTiles;
    QVariantList m_restoredTiles;
    int m_gameId;
    int m_turnId;
    int m_parentTurnId;
    int m_turnIdSequence;
    int m_tileId;
    int m_movingTilesCount;
    MoveDirection m_moveDirection;
    bool m_undoEnabled;
    bool m_undoStarted;
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
  m_gameId(0),
  m_turnId(0),
  m_parentTurnId(0),
  m_turnIdSequence(0),
  m_tileId(0),
  m_movingTilesCount(0),
  m_moveDirection(MoveDirection::None),
  m_undoEnabled(true),
  m_undoStarted(false),
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


bool GameControllerPrivate::useRestoreAnimation() const
{
    return (START_TILES_COUNT == m_restoredTiles.size() && 0 == m_game->score());
}


void GameControllerPrivate::createTile(int value, int cell)
{
    const int id = nextTileId();
    createTile(id, value, cell, true);
}


void GameControllerPrivate::createTile(int id, int value, int cell, bool animation)
{
    Tile_ptr tile;

    if (m_hiddenTiles.empty()) {
        auto tileItem = q_check_ptr(qobject_cast<QQuickItem*>(m_tileQmlComponent->create()));
        tile = std::make_shared<Tile>(id, value, tileItem, q_check_ptr(m_game->tilesParent()));
        QObject::connect(tile.get(), &Tile::moveFinished, q, &GameController::onTileMoveFinished);
    } else {
        tile = m_hiddenTiles.takeLast();
        tile->setId(id);
        tile->setValue(value);
    }

    m_tiles.append(tile);

    tile->setCell(m_cells.at(cell));
    tile->show(animation);
}


void GameControllerPrivate::createRandomTile()
{
    QList<int> cells;

    for (const auto &cell : m_cells) {
        if (!cell->tile()) {
            cells.append(cell->index());
        }
    }

    const int value = random() < 0.9 ? 2 : 4;
    const int cell = cells.at(int(std::floor(random() * (cells.size() - 1))));

    createTile(value, cell);
}


void GameControllerPrivate::createStartTiles()
{
    for (int i = 0; i < START_TILES_COUNT; ++i) {
        createRandomTile();
    }
}


void GameControllerPrivate::hideTile(const Tile_ptr &tile, bool animation)
{
    Q_ASSERT(tile);

    if (!m_hiddenTiles.contains(tile)) {
        m_hiddenTiles.append(tile);
        tile->hide(animation);
        tile->setCell(nullptr);
    } else {
        Q_ASSERT(false);
    }
}


void GameControllerPrivate::moveTile(const Cell_ptr &sourceCell, const Cell_ptr &targetCell)
{
    Q_ASSERT(sourceCell);
    Q_ASSERT(targetCell);

    if (sourceCell != targetCell) {
        const auto &tile = sourceCell->tile();
        Q_ASSERT(tile);
        sourceCell->setTile(nullptr);
        targetCell->setTile(tile);
        Q_ASSERT(tile->cell() == targetCell);
        ++m_movingTilesCount;
    }
}


void GameControllerPrivate::clearTiles()
{
    for (const auto &tile : m_tiles) {
        hideTile(tile, false);
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
    Q_ASSERT(0 != m_gameId);
    Q_ASSERT(0 != m_turnId);

    QVariantMap game;
    game.insert(QLatin1Literal(Internal::GAME_ID_KEY), m_gameId);
    game.insert(QLatin1Literal(Internal::TURN_ID_KEY), m_turnId);
    game.insert(QLatin1Literal(Internal::PARENT_TURN_ID_KEY), m_parentTurnId);
    game.insert(QLatin1Literal(Internal::GAME_STATE_KEY), QVariant::fromValue<GameState>(m_game->gameState()));
    game.insert(QLatin1Literal(Internal::MOVE_DIRECTION_KEY), QVariant::fromValue<MoveDirection>(m_moveDirection));
    game.insert(QLatin1Literal(Internal::SCORE_KEY), m_game->score());
    game.insert(QLatin1Literal(Internal::BEST_SCORE_KEY), m_game->bestScore());
    game.insert(QLatin1Literal(Internal::TILES_KEY), tilesToList());

    m_storage->saveTurn(game);
}


bool GameControllerPrivate::isFirstTurn() const
{
    return FIRST_TURN_ID == m_turnId;
}


QVariantList GameControllerPrivate::tilesToList() const
{
    QVariantList tiles;

    for (const auto &tile : m_tiles) {
        tiles.append(tileToMap(tile));
    }

    return tiles;
}


QVariantMap GameControllerPrivate::tileToMap(const Tile_ptr &tile) const
{
    Q_ASSERT(tile);
    Q_ASSERT(tile->cell());

    QVariantMap map;
    map.insert(QLatin1Literal(TILE_ID_KEY), tile->id());
    map.insert(QLatin1Literal(TILE_VALUE_KEY), tile->value());
    map.insert(QLatin1Literal(TILE_CELL_KEY), tile->cell()->index());

    return map;
}


TileData GameControllerPrivate::tileFromVariant(const QVariant &tile) const
{
    Q_ASSERT(tile.canConvert<QVariantMap>());

    const QVariantMap &map = tile.toMap();

    Q_ASSERT_X(map.contains(QLatin1Literal(TILE_ID_KEY)), "Tile from map", "Tile id key not found");
    Q_ASSERT_X(map.contains(QLatin1Literal(TILE_VALUE_KEY)), "Tile from map", "Tile value key not found");
    Q_ASSERT_X(map.contains(QLatin1Literal(TILE_CELL_KEY)), "Tile from map", "Tile cell key not found");

    const int id = map.value(QLatin1Literal(TILE_ID_KEY)).toInt();
    const int value = map.value(QLatin1Literal(TILE_VALUE_KEY)).toInt();
    const int cell = map.value(QLatin1Literal(TILE_CELL_KEY)).toInt();

    return TileData({ id, value, cell });
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
    connect(d->m_game.get(), &Game::gameReady, this, &GameController::onGameReady);
    connect(d->m_game.get(), &Game::moveTilesRequested, this, &GameController::onMoveTilesRequested);
    connect(d->m_game.get(), &Game::startNewGameRequested, this, &GameController::onStartNewGameRequested);
    connect(d->m_game.get(), &Game::continueGameRequested, this, &GameController::onContinueGameRequested);
    connect(d->m_game.get(), &Game::undoRequested, this, &GameController::onUndoRequested);
    connect(d->m_storage.get(), &Storage::storageReady, this, &GameController::onStorageReady);
    connect(d->m_storage.get(), &Storage::storageError, this, &GameController::onStorageError);
    connect(d->m_storage.get(), &Storage::gameCreated, this, &GameController::onGameCreated);
    connect(d->m_storage.get(), &Storage::createGameError, this, &GameController::onCreateGameError);
    connect(d->m_storage.get(), &Storage::gameRestored, this, &GameController::onGameRestored);
    connect(d->m_storage.get(), &Storage::restoreGameError, this, &GameController::onRestoreGameError);
    connect(d->m_storage.get(), &Storage::turnSaved, this, &GameController::onTurnSaved);
    connect(d->m_storage.get(), &Storage::saveTurnError, this, &GameController::onSaveTurnError);
    connect(d->m_storage.get(), &Storage::turnUndid, this, &GameController::onTurnUndid);
    connect(d->m_storage.get(), &Storage::undoTurnError, this, &GameController::onUndoTurnError);
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
    qDebug() << "Game ready";

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

    d->m_moveBlocked = false;
}


void GameController::onUndoRequested()
{
    if (!d->m_moveBlocked && d->m_undoEnabled && !d->isFirstTurn()) {
        d->m_moveBlocked = true;
        d->m_storage->undoTurn(d->m_turnId);
    }
}


void GameController::onMoveTilesRequested(MoveDirection direction)
{
    if (d->m_moveBlocked || MoveDirection::None == direction) {
        return;
    }

    d->m_moveBlocked = true;
    d->m_moveDirection = direction;

    int rows = 0;
    int columns = 0;

    switch (direction) {
    case MoveDirection::Left:
    case MoveDirection::Right:
        rows = d->m_game->gameboardRows();
        columns = d->m_game->gameboardColumns();
        break;
    case MoveDirection::Up:
    case MoveDirection::Down:
        rows = d->m_game->gameboardColumns();
        columns = d->m_game->gameboardRows();
        break;
    case MoveDirection::None:
        Q_ASSERT(false);
        d->m_moveBlocked = false;
        return;
    }

    for (int row = 0; row < rows; ++row) {
        int firstCellIndex = 0;
        int neighborCellsIndexDelta = 0;

        switch (direction) {
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
            switch (direction) {
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

            const auto &cell = d->m_cells.at(cellIndex);

            if (!cell->tile()) {
                continue;
            }

            auto previousCell = d->m_cells.at(previousCellIndex);
            if (!previousCell->tile()) {
                const auto &tile = cell->tile();
                tile->setZ(0);
                d->moveTile(cell, previousCell);
                continue;
            }

            if (previousCell->tile()->value() == cell->tile()->value()) {
                const auto &tile = cell->tile();
                tile->setValue(tile->value() * 2);
                tile->setZ(previousCell->tile()->z() + 1);
                d->m_aboutToHiddenTiles.append(previousCell->tile());
                d->m_tiles.removeOne(previousCell->tile());
                previousCell->tile()->setCell(nullptr);
                previousCell->setTile(nullptr);
                d->moveTile(cell, previousCell);
                switch (direction) {
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
            switch (direction) {
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
                switch (direction) {
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
                previousCell = d->m_cells.at(previousCellIndex);
                cell->tile()->setZ(0);
                d->moveTile(cell, previousCell);
                continue;
            }

            previousCellIndex = cellIndex;
        }
    }

    d->m_parentTurnId = d->m_turnId;
    d->m_turnId = ++d->m_turnIdSequence;

    if (0 == d->m_movingTilesCount)
      d->m_moveBlocked = false;
}


void GameController::onTileMoveFinished()
{
    Q_ASSERT(d->m_movingTilesCount >= 0);

    static bool win = false;

    Tile *tile = q_check_ptr(qobject_cast<Tile*>(sender()));

    if (WINNING_VALUE == tile->value() && GameState::Continue != d->m_game->gameState()) {
        win = true;
    }

    if (0 == --d->m_movingTilesCount) {
        int score = d->m_game->score();
        for (const auto &tile : d->m_aboutToHiddenTiles) {
            score += tile->value() * 2;
            d->hideTile(tile, false);
        }

        d->m_aboutToHiddenTiles.clear();
        d->m_game->setScore(score);

        bool moveBlocked = false;

        if (win) {
            d->m_game->setGameState(GameState::Win);
            moveBlocked = true;
            win = false;
        } else {
            d->createRandomTile();
        }

        if (d->isDefeat()) {
            d->m_game->setGameState(GameState::Defeat);
            moveBlocked = true;
        }

        if (StorageState::Ready == d->m_storage->state()) {
            d->saveTurn();
        }

        if (!moveBlocked) {
            d->m_moveBlocked = false;
        }
    }
}


void GameController::onStorageReady()
{
    qDebug() << "Storage ready";

    if (d->m_game->isReady()) {
        d->m_storage->restoreGame();
    }

}


void GameController::onStorageError()
{
    qWarning() << "Storage error";

    if (d->m_game->isReady()) {
        d->createNewGame(DEFAULT_GAMEBOARD_ROWS, DEFAULT_GAMEBOARD_COLUMNS);
    }
}


void GameController::onGameCreated(const QVariantMap &game)
{
    d->m_gameId = game.value(QLatin1Literal(Internal::GAME_ID_KEY)).toInt();

    const int delay = (GameState::Init == d->m_game->gameState()) ? SHOW_START_TILES_DELAY : 0;
    QTimer::singleShot(delay, this, SLOT(startNewGame()));
}


void GameController::onCreateGameError()
{
    const int delay = (GameState::Init == d->m_game->gameState()) ? SHOW_START_TILES_DELAY : 0;
    QTimer::singleShot(delay, this, SLOT(startNewGame()));
}


void GameController::onGameRestored(const QVariantMap &game)
{
    Q_ASSERT(GameState::Init == d->m_game->gameState());
    Q_ASSERT(!d->m_game->isVisible());

    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::GAME_ID_KEY)), "Restore game", "Game id key not found");
    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::TURN_ID_KEY)), "Restore game", "Turn id key not found");
    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::PARENT_TURN_ID_KEY)), "Restore game", "Parent turn id key not found");
    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::MAX_TURN_ID_KEY)), "Restore game", "Max turn id key not found");
    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::TILES_KEY)), "Restore game", "Tiles key not found");
    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::ROWS_KEY)), "Restore game", "Rows key not found");
    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::COLUMNS_KEY)), "Restore game", "Columns key not found");
    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::SCORE_KEY)), "Restore game", "Score key not found");
    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::BEST_SCORE_KEY)), "Restore game", "Best score key not found");
    Q_ASSERT_X(game.contains(QLatin1Literal(Internal::GAME_STATE_KEY)), "Restore game", "Game state key not found");

    d->m_gameId = game.value(QLatin1Literal(Internal::GAME_ID_KEY)).toInt();
    d->m_turnId = game.value(QLatin1Literal(Internal::TURN_ID_KEY)).toInt();
    d->m_parentTurnId = game.value(QLatin1Literal(Internal::PARENT_TURN_ID_KEY)).toInt();
    d->m_turnIdSequence = game.value(QLatin1Literal(Internal::MAX_TURN_ID_KEY)).toInt();
    d->m_restoredTiles = game.value(QLatin1Literal(Internal::TILES_KEY)).toList();
    d->setGameboardSize(game.value(QLatin1Literal(Internal::ROWS_KEY)).toInt(),
                        game.value(QLatin1Literal(Internal::COLUMNS_KEY)).toInt());
    d->m_game->setScore(game.value(QLatin1Literal(Internal::SCORE_KEY)).toInt());
    d->m_game->setBestScore(game.value(QLatin1Literal(Internal::BEST_SCORE_KEY)).toInt());
    d->m_game->setGameState(game.value(QLatin1Literal(Internal::GAME_STATE_KEY)).value<GameState>());
    d->m_game->setUndoButtonEnabled(!d->isFirstTurn(), false);
    d->m_game->show();

    Q_ASSERT(0 < d->m_game->gameboardRows() && 0 < d->m_game->gameboardColumns());
    Q_ASSERT(!d->m_restoredTiles.isEmpty());

    const int timeout = d->useRestoreAnimation() ? SHOW_START_TILES_DELAY : 0;
    QTimer::singleShot(timeout, this, SLOT(restoreGame()));
}


void GameController::onRestoreGameError()
{
    Q_ASSERT(!d->m_game->isVisible());

    d->createNewGame(DEFAULT_GAMEBOARD_ROWS, DEFAULT_GAMEBOARD_COLUMNS);
    d->m_game->show();
}


void GameController::onTurnSaved()
{
    if (d->m_undoEnabled && !d->isFirstTurn() && !d->m_game->isUndoButtonEnabled()) {
        d->m_game->setUndoButtonEnabled(true);
    }
}


void GameController::onSaveTurnError()
{
    if (d->m_undoEnabled) {
        d->m_undoEnabled = false;
        d->m_game->setUndoButtonEnabled(false);
    }
}


void GameController::onTurnUndid(const QVariantMap &turn)
{
    // TODO: realisation
    d->m_moveBlocked = false;
}


void GameController::onUndoTurnError()
{
    d->m_undoEnabled = false;
    d->m_game->setUndoButtonEnabled(false);
    d->m_moveBlocked = false;
}


void GameController::startNewGame()
{
    d->m_game->setScore(0);
    d->m_game->setGameState(GameState::Play);
    d->m_game->setUndoButtonEnabled(false);
    d->m_moveDirection = MoveDirection::None;
    d->m_turnId = FIRST_TURN_ID;
    d->m_parentTurnId = FIRST_PARENT_TURN_ID;
    d->m_turnIdSequence = FIRST_TURN_ID;
    d->m_undoEnabled = true;
    d->clearTiles();
    d->createStartTiles();

    if (StorageState::Ready == d->m_storage->state()) {
        d->saveTurn();
    }

    d->m_moveBlocked = false;
}


void GameController::restoreGame()
{
    Q_ASSERT(!d->m_restoredTiles.isEmpty());

    int maxValue = 0;
    const bool animation = d->useRestoreAnimation();

    for (const QVariant &var : d->m_restoredTiles) {
        const auto &tile = d->tileFromVariant(var);
        d->createTile(tile.id, tile.value, tile.cell, animation);
        if (maxValue < tile.value) {
            maxValue = tile.value;
        }
    }

    const GameState gameState = d->m_game->gameState();
    const bool winOrContinue = (GameState::Continue == gameState || GameState::Win == gameState);

    if (d->isDefeat() && GameState::Defeat != gameState) {
        d->m_game->setGameState(GameState::Defeat);
    } else if (winOrContinue && maxValue < WINNING_VALUE) {
        d->m_game->setGameState(GameState::Play);
    } else if (GameState::Init == gameState) {
        d->m_game->setGameState(GameState::Play);
    }

    if (GameState::Win != gameState && GameState::Defeat != gameState) {
        d->m_moveBlocked = false;
    }

    d->m_restoredTiles.clear();
}

} // namespace Game
