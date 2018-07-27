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


#include "storage.h"
#include "storageworker.h"

#include <QDebug>
#include <QThread>

static const int WORKER_THREAD_QUIT_TIMEOUT = 3000;


namespace Game {
namespace Internal {

using StorageState = Storage::StorageState;
using TileSpecs = QList<TileSpec>;

class StoragePrivate final
{
public:
    explicit StoragePrivate(Storage *parent);
    ~StoragePrivate();

    void openDatabase();
    void closeDatabase();

    Storage *const q;

    const std::unique_ptr<QThread> m_workerThread;
    const std::unique_ptr<StorageWorker> m_worker;
    StorageState m_state;
};


StoragePrivate::StoragePrivate(Storage *parent) :
    q(parent),
    m_workerThread(std::make_unique<QThread>(parent)),
    m_worker(std::make_unique<StorageWorker>()),
    m_state(Storage::StorageState::NotReady)
{
    qRegisterMetaType<GameSpec>("GameSpec");
    qRegisterMetaType<TurnSpec>("TurnSpec");

    m_worker->moveToThread(m_workerThread.get());

    QObject::connect(m_worker.get(), &StorageWorker::storageReady, q, &Storage::onStorageReady);
    QObject::connect(m_worker.get(), &StorageWorker::storageError, q, &Storage::onStorageError);

    QObject::connect(m_worker.get(), &StorageWorker::gameCreated, q, &Storage::gameCreated);
    QObject::connect(m_worker.get(), &StorageWorker::createGameError, q, &Storage::createGameError);

    QObject::connect(m_worker.get(), &StorageWorker::gameRestored, q, &Storage::gameRestored);
    QObject::connect(m_worker.get(), &StorageWorker::restoreGameError, q, &Storage::restoreGameError);

    QObject::connect(m_worker.get(), &StorageWorker::turnSaved, q, &Storage::turnSaved);
    QObject::connect(m_worker.get(), &StorageWorker::saveTurnError, q, &Storage::saveTurnError);

    QObject::connect(m_worker.get(), &StorageWorker::turnUndid, q, &Storage::turnUndid);
    QObject::connect(m_worker.get(), &StorageWorker::undoTurnError, q, &Storage::undoTurnError);

    m_workerThread->start();
}


StoragePrivate::~StoragePrivate()
{
    closeDatabase();
    m_workerThread->quit();
    if (!m_workerThread->wait(WORKER_THREAD_QUIT_TIMEOUT)) {
        qWarning() << "Failed to quit from storage worker thread";
        m_workerThread->terminate();
    }
}


void StoragePrivate::openDatabase()
{
    QMetaObject::invokeMethod(m_worker.get(), "openDatabase", Qt::QueuedConnection);
}


void StoragePrivate::closeDatabase()
{
    QMetaObject::invokeMethod(m_worker.get(), "closeDatabase", Qt::QueuedConnection);
}


Storage::Storage(QObject *parent) :
    QObject(parent),
    d(std::make_unique<StoragePrivate>(this))
{
}


Storage::~Storage()
{
}


void Storage::init()
{
    d->openDatabase();
}


StorageState Storage::state() const
{
    return d->m_state;
}


void Storage::createGame(int rows, int columns)
{
    QMetaObject::invokeMethod(d->m_worker.get(), "createGame", Qt::QueuedConnection,
                              Q_ARG(int, rows), Q_ARG(int, columns));
}


void Storage::restoreGame()
{
    QMetaObject::invokeMethod(d->m_worker.get(), "restoreGame", Qt::QueuedConnection);
}


void Storage::saveTurn(const TurnSpec &turn)
{
    QMetaObject::invokeMethod(d->m_worker.get(), "saveTurn", Qt::QueuedConnection, Q_ARG(TurnSpec, turn));
}


void Storage::undoTurn()
{
    QMetaObject::invokeMethod(d->m_worker.get(), "undoTurn", Qt::QueuedConnection);
}


void Storage::onStorageReady()
{
    d->m_state = StorageState::Ready;
    emit storageReady();
}


void Storage::onStorageError()
{
    d->m_state = StorageState::Error;
    emit storageError();
}

} // namespace Internal
} // namespace Game
