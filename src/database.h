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
