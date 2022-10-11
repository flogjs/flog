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

#include "base.h"
#include "../string.h"
#include "../app.h"
    
//flog::Database* database;

// <file>.js, sync, list
static int args2(int argc, char* argv[]) {
  if (argc != 2) {
    return -1;
  }

  char* operation = argv[1];

  if (flog_string_endswith(operation, ".js")) {
    return flog_command_file(operation, argc, argv);
  }

  if (flog_string_equals(operation, "sync")) {
    return flog_command_sync();
  }

  if (flog_string_equals(operation, "list")) {
    return flog_command_list();
  }

  return 0;
}

// sync, info, with
static int args3(int argc, char* argv[]) {
  if (argc != 3) {
    return -1;
  }

  char* operation = argv[1];
  char* param = argv[2];

  if (flog_string_equals(operation, "sync")) {
    return flog_command_sync(param);
  }

  if (flog_string_equals(operation, "info")) {
    return flog_command_info(param);
  }

  if (flog_string_equals(operation, "with")) {
    return flog_command_with(param);
  }

  return 0;
}

int flog_command_args(int argc, char* argv[]) {
  FlogApp* app = flog_app_new();

  int result = argc == 2 ? args2(argc, argv) : args3(argc, argv);

  flog_app_dispose(app);

  return 0;
}
