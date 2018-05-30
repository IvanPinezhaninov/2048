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


#include "gamecontroller.h"
#include "logger.h"

#include <QDebug>
#include <QGuiApplication>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
#include <QSysInfo>
#endif

#ifdef QT_DEBUG
static const auto defaultMessageHandler = qInstallMessageHandler(nullptr);
#endif


static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    Log::Logger::instance().write(type, context, message);
#ifdef QT_DEBUG
    (*defaultMessageHandler)(type, context, message);
#endif
}


int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageHandler);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setOrganizationName(QLatin1Literal(ORGANIZATION_NAME));
    app.setApplicationName(QLatin1Literal(APPLICATION_NAME));

    qDebug() << "Application opened";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    qDebug().noquote() << "Host system:" << QSysInfo::prettyProductName() << QSysInfo::currentCpuArchitecture();
#endif

    auto result = EXIT_FAILURE;

    {
        using GameController = Game::GameController;
        GameController gameController;
        QObject::connect(&app, &QGuiApplication::lastWindowClosed, &gameController, &GameController::shutdown);

        if (gameController.init()) {
            result = app.exec();
        } else {
            qDebug() << "The application was unable to start correctly";
        }
    }

    qDebug() << "Application closed";
    return result;
}
