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

#include "database.h"
#include "string.h"
#include "file.h"
#include "flog.h"

static void init_repo(const char* home) {
  char* path = flog_string_glue(home, "repo");
  const char* repo_path = "https://github.com/flogjs/sync";
  if (!flog_file_exists(path)) {
    status("Initializing database at ~/.flog/repo");
    git_repository* repo = NULL;
    int ret = git_clone(&repo, repo_path, path, NULL);
    git_repository_free(repo);
  }
  free(path);
}

static void init_home(const char* home) {
  RETURN_IF(flog_file_exists(home));
  mkdir(home, 0700);
  init_repo(home);
}

Database* flog_init_database() {
  const char* homedir = getpwuid(getuid())->pw_dir;
  Database* database = calloc(1, sizeof(* database));
  git_libgit2_init();
  database->home = flog_string_glue(homedir, ".flog");
  
  init_home(database->home);

  return database;
}

void flog_teardown_database(Database* database) {
  git_libgit2_shutdown();
  free(database->home);
  free(database);
}
