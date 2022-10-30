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
#ifndef FLOG_MODULE_H_
#define FLOG_MODULE_H_

#include <dlfcn.h>
#include "string.h"
#include "file.h"
#include "engine.h"

struct Module {
  struct Module* next;
};
typedef struct Module Module;

typedef JSModuleDef* (*Export)(JSContext* context, const char* name);
typedef JSModuleDef* (*Loader)(JSContext* context, const char* path, bool main);

JSModuleDef* flog_load_module(JSContext* context,
                              const char* name,
                              void* opaque);
JSModuleDef* flog_load_main_module(JSContext* context, const char* name);
JSModuleDef* flog_run_code(JSContext*, char*);
#endif
