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

#ifndef FLOG_APP_H_
#define FLOG_APP_H_
#include <stdarg.h>
#include <stdlib.h>

#include "engine.h"
#include "module.h"
#include "database.h"

typedef struct App {
  Engine* engine;
  Database* database;
  Module* modules;
} App;

void flog_app_new();
void flog_app_free();
void flog_add_module(Module* mod);
#endif
