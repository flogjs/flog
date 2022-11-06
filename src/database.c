/* flog
 * Copyright (C) 2022 <terrablue@posteo.is>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "git.h"
#include "database.h"
#include "string.h"
#include "file.h"
#include "flog.h"

static void init_repo(const char* home) {
  char* path = flog_string_glue(home, "repo");
  const char* url = "https://github.com/flogjs/sync";
  if (!flog_file_exists(path)) {
    info("Initializing database at ~/.flog/repo\n");
    flog_git_clone(url, path);
  }
  free(path);
}

static void init_home(const char* home) {
  RETURN_IF(flog_file_exists(home));
  mkdir(home, 0700);
  init_repo(home);
}

Database* flog_database_new() {
  const char* homedir = getpwuid(getuid())->pw_dir;
  Database* database = calloc(1, sizeof(* database));
  flog_git_init();
  database->home = flog_string_glue(homedir, ".flog");
  
  init_home(database->home);

  char* path = flog_string_glue(database->home, "repo");
  database->master_db = flog_string_glue(path, "master.db");
  flog_git_open(database, path);
  free(path);

  return database;
}

void flog_database_free(Database* database) {
  flog_git_close(database);
  flog_git_free();
  free(database->home);
  free(database->master_db);
  free(database);
}

static size_t count_modules(Database* database) {
  char* repo = flog_string_glue(database->home, "repo");
  char* master_db = flog_string_glue(repo, "master.db");
  size_t count = flog_file_n_lines((const char*) master_db);
  free(master_db);
  free(repo);
  return count;
}

void flog_database_sync(Database* database) {
  info("Syncing database...");
  int error = git_remote_fetch(database->remote, NULL, NULL, NULL);

  more(flog_git_pull_master(database) ? " done" : " nothing to do");
}

bool flog_database_has(Database* database, const char* name) {
  return flog_file_match_module_line(database->master_db, name);
}
