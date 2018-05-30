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


#include "logger.h"
#include "loggerworker.h"

#include <QThread>

static const int WORKER_THREAD_QUIT_TIMEOUT = 3000;


namespace Log {
namespace Internal {

class LoggerPrivate final
{
public:
    explicit LoggerPrivate();
    ~LoggerPrivate();

    void openFile();
    void closeFile();

    const std::unique_ptr<QThread> m_workerThread;
    const std::unique_ptr<LoggerWorker> m_worker;
};


LoggerPrivate::LoggerPrivate() :
    m_workerThread(std::make_unique<QThread>()),
    m_worker(std::make_unique<LoggerWorker>())
{
    qRegisterMetaType<QtMsgType>("QtMsgType");
}


LoggerPrivate::~LoggerPrivate()
{
    m_workerThread->quit();
    if (!m_workerThread->wait(WORKER_THREAD_QUIT_TIMEOUT)) {
        m_workerThread->terminate();
    }
}


void LoggerPrivate::openFile()
{
    QMetaObject::invokeMethod(m_worker.get(), "openFile", Qt::QueuedConnection);
}


void LoggerPrivate::closeFile()
{
    QMetaObject::invokeMethod(m_worker.get(), "closeFile", Qt::QueuedConnection);
}

} // namespace Internal


Logger &Logger::instance()
{
    static const std::unique_ptr<Logger> instance(new Logger());
    return *instance;
}


void Logger::write(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    write(type, context.file, context.line, context.function, context.category, message);
}


void Logger::write(QtMsgType type, const QString &file, int line, const QString &function, const QString &category, const QString &message)
{
    QMetaObject::invokeMethod(instance().d->m_worker.get(), "write", Qt::QueuedConnection,
                              Q_ARG(QtMsgType, type), Q_ARG(QString, file), Q_ARG(int, line),
                              Q_ARG(QString, function), Q_ARG(QString, category), Q_ARG(QString, message));
}


Logger::Logger() :
    d(std::make_unique<Internal::LoggerPrivate>())
{
    d->m_worker->moveToThread(d->m_workerThread.get());
    d->m_workerThread->start();
    d->openFile();
}


Logger::~Logger()
{
    d->closeFile();
}

} // namespace Log
