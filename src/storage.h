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


#ifndef STORAGE_H
#define STORAGE_H

#include <memory>

#include <QObject>

#include "gamespec.h"
#include "tilespec.h"
#include "turnspec.h"


namespace Game {
namespace Internal {

class StoragePrivate;

class Storage final : public QObject
{
    Q_OBJECT
public:
    enum class StorageState : quint8
    {
        Ready,
        NotReady,
        Error
    };

    explicit Storage(QObject *parent = nullptr);
    ~Storage();

    void init();

    StorageState state() const;

signals:
    void storageReady();
    void storageError();

    void gameCreated(const GameSpec &game);
    void createGameError();

    void turnSaved();
    void saveTurnError();

    void gameRestored(const GameSpec &game);
    void restoreGameError();

public slots:
    void createGame(int rows, int columns);
    void restoreGame();
    void saveTurn(const TurnSpec &turn);

private slots:
    void onStorageReady();
    void onStorageError();

private:
    Q_DISABLE_COPY(Storage)

    const std::unique_ptr<StoragePrivate> d;

    friend class StoragePrivate;
};

} // namespace Internal
} // namespace Game

#endif // STORAGE_H
