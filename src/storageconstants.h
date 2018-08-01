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


#ifndef STORAGECONSTANTS_H
#define STORAGECONSTANTS_H

namespace Game {
namespace Internal {

const char *const BEST_SCORE_KEY = "bestScore";
const char *const COLUMNS_KEY = "columns";
const char *const GAME_ID_KEY = "gameId";
const char *const GAME_STATE_KEY = "gameState";
const char *const MAX_TURN_ID_KEY = "maxKeyId";
const char *const MOVE_DIRECTION_KEY = "moveDirection";
const char *const PARENT_TURN_ID_KEY = "parentTurnId";
const char *const ROWS_KEY = "rows";
const char *const SCORE_KEY = "score";
const char *const TILE_CELL_KEY = "tileCell";
const char *const TILE_ID_KEY = "tileId";
const char *const TILE_VALUE_KEY = "tileValue";
const char *const TILES_KEY = "tiles";
const char *const TURN_ID_KEY = "turnId";
const char *const UNDO_CREATED_TILES_KEY = "created";
const char *const UNDO_REGULAR_TILES_KEY = "regular";
const char *const UNDO_REMOVED_TILES_KEY = "removed";

} // namespace Internal
} // namespace Game

#endif // STORAGECONSTANTS_H
