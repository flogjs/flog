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

#define META_REPO "meta"

static void init_modules_directory(const char* home) {
  char* modules = flog_string_glue(home, "modules");
  NEXT_IF(flog_file_exists(modules));

  mkdir(modules, 0744);

  next:
    free(modules);
}

static void init_meta_repo(const char* home) {
  char* path = flog_string_glue(home, META_REPO);
  const char* url = "https://github.com/flogjs/sync";
  NEXT_IF(flog_file_exists(path));

  info("Initializing database at ~/.flog/%s", META_REPO);
  flog_git_clone(url, path);

  next:
    free(path);
}

static void init_home(const char* home) {
  NEXT_IF(flog_file_exists(home));
  mkdir(home, 0700);

  next:
    init_meta_repo(home);
    init_modules_directory(home);
}

Database* flog_database_new() {
  const char* homedir = getpwuid(getuid())->pw_dir;
  Database* database = calloc(1, sizeof(* database));
  flog_git_init();
  database->home = flog_string_glue(homedir, ".flog");
  
  init_home(database->home);

  char* path = flog_string_glue(database->home, META_REPO);
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
  char* repo = flog_string_glue(database->home, META_REPO);
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

void flog_database_clone_module(Database* database, const char* module) {
  int index = flog_string_index_of(module, '/');
  char* namespace = flog_string_slice(module, 0, index);
  char namespace_directory[1000];
  sprintf(namespace_directory, "%s/modules/%s", database->home, namespace);
  free(namespace);
  if (!flog_file_exists(namespace_directory)) {
    mkdir(namespace_directory, 0744);
  }

  char path[1000];
  sprintf(path, "%s/modules/%s", database->home, module);

  if (!flog_file_exists(path)) {
    char base[] = "https://github.com/flogjs";
    char url[100];
    sprintf(url, "%s/%s-%s-sync", base, "std", "console");
    // init clone the repo
    flog_git_clone_bare(url, path);
  }
}

void flog_database_checkout_module(Database* database,
                                   const char* module,
                                   const char* working_directory) {
  // todo: check if repo has updates
  RETURN_IF(flog_file_exists(working_directory));
  char url[1000];
  sprintf(url, "file://%s/modules/%s", database->home, module);
  flog_git_clone(url, working_directory);
}

bool flog_database_has(Database* database, const char* name) {
  return flog_file_match_module_line(database->master_db, name);
}
