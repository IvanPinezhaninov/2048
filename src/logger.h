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


#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>

#include <memory>


namespace Log {
namespace Internal {
class LoggerPrivate;
} // namespace Internal

class Logger final
{
public:
    static Logger &instance();
    void write(QtMsgType type, const QMessageLogContext &context, const QString &message);
    void write(QtMsgType type, const QString &file, int line,
               const QString &function, const QString &category, const QString &message);

private:
    Logger();
    ~Logger();

    Logger(const Logger &other) = delete;
    Logger &operator=(const Logger &other) = delete;

    const std::unique_ptr<Internal::LoggerPrivate> d;

    friend std::unique_ptr<Logger>::deleter_type;
};

} // namespace Log

#endif // LOGGER_H
