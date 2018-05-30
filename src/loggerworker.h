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


#ifndef LOGGERWORKER_H
#define LOGGERWORKER_H

#include <QFile>
#include <QMutex>
#include <QObject>

namespace Log {
namespace Internal {

class LoggerWorker final : public QObject
{
    Q_OBJECT
public:
    LoggerWorker();

public slots:
    void openFile();
    void closeFile();
    void write(QtMsgType type, const QString &file, int line, const QString &function, const QString &category, const QString &message);

private:
    Q_DISABLE_COPY(LoggerWorker)


    QString typeToString(QtMsgType type) const;

    QFile m_file;
    QMutex m_lock;
};

} // namespace Log
} // namespace Internal

#endif // LOGGERWORKER_H
