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
#include "gamecontroller.h"

#include <QGuiApplication>
#include <QScreen>
#include <QSettings>

static const char *const GAME_WINDOW_X_SETTING_KEY_NAME = "x";
static const char *const GAME_WINDOW_Y_SETTING_KEY_NAME = "y";
static const char *const GAME_WINDOW_WIDTH_SETTING_KEY_NAME = "width";
static const char *const GAME_WINDOW_HEIGHT_SETTING_KEY_NAME = "height";
static const char *const GAME_BEST_SCORE_SETTING_KEY_NAME = "bestScore";

#ifdef Q_OS_MACOS
static const char *const SETTINGS_FILE_LOCATION = "%1/../Resources/settings.ini";
#endif

namespace Game {
namespace Internal {

class GameControllerPrivate
{
public:
    GameControllerPrivate(GameController* parent);

    void readSettings();
    void saveSettings();

    GameController *const q;
    const std::unique_ptr<Game> m_game;
    const std::unique_ptr<QSettings> m_settings;
};


GameControllerPrivate::GameControllerPrivate(GameController *parent) :
    q(parent),
    m_game(std::make_unique<Game>(parent)),
#ifdef Q_OS_MACOS
    m_settings(std::make_unique<QSettings>(QString(QLatin1Literal(SETTINGS_FILE_LOCATION))
                                           .arg(QCoreApplication::applicationDirPath()), QSettings::IniFormat))
#else
    m_settings(std::make_unique<QSettings>())
#endif
{
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

    // Game best score
    m_game->setBestScore(m_settings->value(QLatin1Literal(GAME_BEST_SCORE_SETTING_KEY_NAME)).toInt());
}


void GameControllerPrivate::saveSettings()
{
    // Game window geometry
    const QRect &rect = m_game->geometry();
    m_settings->setValue(QLatin1Literal(GAME_WINDOW_X_SETTING_KEY_NAME), rect.x());
    m_settings->setValue(QLatin1Literal(GAME_WINDOW_Y_SETTING_KEY_NAME), rect.y());
    m_settings->setValue(QLatin1Literal(GAME_WINDOW_WIDTH_SETTING_KEY_NAME), rect.width());
    m_settings->setValue(QLatin1Literal(GAME_WINDOW_HEIGHT_SETTING_KEY_NAME), rect.height());

    // Game best score
    m_settings->setValue(QLatin1Literal(GAME_BEST_SCORE_SETTING_KEY_NAME), m_game->bestScore());
}

} // namespace Internal


GameController::GameController(QObject *parent) :
    QObject(parent),
    d(std::make_unique<Internal::GameControllerPrivate>(this))
{
}


GameController::~GameController()
{
}


bool GameController::start()
{
    if (!d->m_game->init()) {
        return false;
    }

    d->readSettings();
    d->m_game->show();

    return true;
}


void GameController::shutdown()
{
    d->saveSettings();
}

} // namespace Game
