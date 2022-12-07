/*
 * Flog JavaScript Runtime
 * 
 * Copyright (c) Terrablue <terrablue@proton.me> and contributors.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
    // prepublished modules until we have our server and `flog publish`
    char* prepubs[] = {"std/console", "std/dyndef"};
    for (size_t i = 0; i < sizeof(prepubs) / sizeof(prepubs[0]); i++) {
      if (flog_string_equals(module, prepubs[i])) {
        char base[] = "https://github.com/flogjs";
        char url[100];
        int index = flog_string_index_of(module, '/');
        char* namespace = flog_string_slice(module, 0, index);
        int length = flog_string_length(module);
        char* name = flog_string_slice(module, index + 1, length);
        sprintf(url, "%s/%s-%s-sync", base, namespace, name);
        free(namespace);
        free(name);
        // init clone the repo
        flog_git_clone_bare(url, path);
      }
    }

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
