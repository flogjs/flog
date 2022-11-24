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

#ifndef FLOG_DATABASE_H_
#define FLOG_DATABASE_H_

#define DB_FILE = "master.db";

#include <pwd.h>
#include <unistd.h>
#include <stdbool.h>
#include <git2.h>
#include <sys/stat.h>
#include "module.h"

typedef struct Entry {
  char* name;
  char* version;
  struct Entry* next;
} Entry;

typedef struct Database {
  char* home;
  char* master_db;
  git_repository* repo;
  git_remote* remote;
  Entry* entries;
} Database;

Database* flog_database_new();
void flog_database_free(Database*);
void flog_database_sync(Database*);
void flog_database_clone_module(Database*, const char*);
void flog_database_checkout_module(Database*, const char*, const char*);
bool flog_database_has(Database*, const char*);

#endif
