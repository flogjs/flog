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

#include <s4/cutils.h>
#include <s4/quickjs.h>
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
