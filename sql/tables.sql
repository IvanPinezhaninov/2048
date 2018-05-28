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
                          (turn_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
                           turn_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
                           game_id INTEGER NOT NULL,
                           parent_turn_id INTEGER NOT NULL,
                           move_direction INTEGER NOT NULL,
                           score INTEGER NOT NULL,
                           best_score INTEGER NOT NULL,
                           FOREIGN KEY (game_id) REFERENCES games(game_id));

CREATE TABLE IF NOT EXISTS tiles
                          (tile_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
                           turn_id INTEGER NOT NULL,
                           tile_spec_id INTEGER NOT NULL,
                           tile_spec_cell INTEGER NOT NULL,
                           tile_spec_value INTEGER NOT NULL,
                           FOREIGN KEY (turn_id) REFERENCES turns(turn_id));

PRAGMA user_version = 1;
