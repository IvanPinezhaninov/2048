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


#ifndef GAMESPEC_H
#define GAMESPEC_H

#include <memory>

#include <QList>
#include <QMetaType>

namespace Game {
namespace Internal {

class GameSpecPrivate;
class TileSpec;

class GameSpec final
{
public:
    GameSpec();
    GameSpec(int rows, int columns, int score, int bestScore, const QList<TileSpec> &tiles);

    int rows() const;
    int columns() const;
    int score() const;
    int bestScore() const;
    QList<TileSpec> tiles() const;

private:
    std::shared_ptr<GameSpecPrivate> d;
};

} // namespace Internal
} // namespace Game

Q_DECLARE_METATYPE(Game::Internal::GameSpec)

#endif // GAMESPEC_H
