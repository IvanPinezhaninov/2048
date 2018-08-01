CREATE TABLE IF NOT EXISTS games
                          (game_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
                           start_time DATETIME DEFAULT CURRENT_TIMESTAMP,
                           finish_time DATETIME DEFAULT CURRENT_TIMESTAMP,
                           rows INTEGER NOT NULL,
                           columns INTEGER NOT NULL,
                           score INTEGER NOT NULL DEFAULT 0,
                           best_score INTEGER NOT NULL DEFAULT 0,
                           game_state INTEGER NOT NULL DEFAULT 1);

CREATE TABLE IF NOT EXISTS turns
                          (turn_id INTEGER PRIMARY KEY NOT NULL,
                           parent_turn_id INTEGER NOT NULL,
                           turn_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
                           move_direction INTEGER NOT NULL,
                           score INTEGER NOT NULL,
                           best_score INTEGER NOT NULL);

CREATE TABLE IF NOT EXISTS tiles
                          (turn_id INTEGER NOT NULL,
                           tile_id INTEGER NOT NULL,
                           tile_value INTEGER NOT NULL,
                           cell_index INTEGER NOT NULL,
                           FOREIGN KEY (turn_id) REFERENCES turns(turn_id));

CREATE INDEX tiles_turn_id ON tiles(turn_id);

CREATE INDEX tiles_cell_index ON tiles(cell_index);

PRAGMA user_version = 1;
