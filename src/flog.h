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
#ifndef FLOG_FLOG_H_
#define FLOG_FLOG_H_

#define COLOR_BOLD "\e[1m"
#define COLOR_RED  "\e[1;31m"
#define COLOR_GREEN "\e[1;32m"
#define COLOR_BLUE "\e[1;34m"
#define OFF  "\e[m"

#define MODULES "modules"

#include <libgen.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include "../deps/quickjs/cutils.h"
#include "../deps/quickjs/quickjs.h"
#include "commands/base.h"
#include "engine.h"
#include "database.h"
#include "module.h"

JSContext* flog_context();
Database* flog_database();

#define SET_STRING(object, property, value)\
  do {\
    JSContext* context = flog_context();\
    JS_SetPropertyStr(context, object, property, JS_NewString(context, value));\
  } while (0);

void info(const char*, ...);
void more(const char*, ...);
void error(const char*, ...);

#define RETURN_IF(predicate)\
  if (predicate) {\
    return;\
  }\

#define NEXT_IF(predicate)\
  if (predicate) {\
    goto next;\
  }

/*#define flog_export(mod, func)\
  void flog_export_##modname(JSContext* context) {\
    func(context);\
  }\*/

#endif
