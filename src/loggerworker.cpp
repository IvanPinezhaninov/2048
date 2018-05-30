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


#include "loggerworker.h"

#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>

static const char *const LOG_FILE_LOCATION = "%1/log_%2.txt";
static const char *const LOG_FILE_DATE_FORMAT = "yyyyMMdd";
static const char *const DATE_TIME_FORMAT = "yyyy.MM.dd hh:mm:ss";
static const char *const DEBUG_MSG_TYPE = "DEBUG";
static const char *const WARNING_MSG_TYPE = "WARNING";
static const char *const CRITICAL_MSG_TYPE = "CRITICAL";
static const char *const FATAL_MSG_TYPE = "FATAL";
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
static const char *const INFO_MSG_TYPE = "INFO";
#endif


namespace Log {
namespace Internal {

LoggerWorker::LoggerWorker() :
    QObject(nullptr)
{
}


void LoggerWorker::openFile()
{
    Q_ASSERT(!m_file.isOpen());

#if (QT_VERSION < QT_VERSION_CHECK(5, 4, 0))
    const QString &logDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#else
    const QString &logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
#endif

    const QString &date = QDate::currentDate().toString(QLatin1Literal(LOG_FILE_DATE_FORMAT));
    const QString &logFileName = QString(QLatin1Literal(LOG_FILE_LOCATION)).arg(logDir, date);

    const QDir dir(logDir);
    if (!dir.exists() && !dir.mkpath(logDir)) {
        return;
    }

    m_file.setFileName(logFileName);
    m_file.open(QIODevice::WriteOnly | QIODevice::Append);
}


void LoggerWorker::closeFile()
{
    m_file.close();
}


void LoggerWorker::write(QtMsgType type, const QString &file, int line,
                         const QString &function, const QString &category, const QString &message)
{
    Q_UNUSED(category)

    QMutexLocker locker(&m_lock);

    if (!m_file.isOpen() || !m_file.isWritable()) {
        return;
    }

    const QString &dateTime = QDateTime::currentDateTime().toString(QLatin1Literal(DATE_TIME_FORMAT));
#ifdef QT_DEBUG
    const QString &msg = QString(QLatin1Literal("%1 %2: %3 (%4:%5, %6)"))
                         .arg(dateTime, typeToString(type), message, file).arg(line).arg(function);
#else
    Q_UNUSED(file)
    Q_UNUSED(line)
    Q_UNUSED(function)
    const QString &msg = QString(QLatin1Literal("%1 %2: %3")).arg(dateTime, typeToString(type), message);
#endif

    QTextStream textStream(&m_file);
    textStream << msg << endl;

    if (QtFatalMsg == type) {
        textStream.flush();
        abort();
    }
}


QString LoggerWorker::typeToString(QtMsgType type) const
{
    switch (type) {
    case QtDebugMsg:
        return QLatin1Literal(DEBUG_MSG_TYPE);
    case QtWarningMsg:
        return QLatin1Literal(WARNING_MSG_TYPE);
    case QtCriticalMsg:
        return QLatin1Literal(CRITICAL_MSG_TYPE);
    case QtFatalMsg:
        return QLatin1Literal(FATAL_MSG_TYPE);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
    case QtInfoMsg:
        return QLatin1Literal(INFO_MSG_TYPE);
#endif
    }
}

} // namespace Log
} // namespace Internal
