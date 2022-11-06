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

#define MODULE_JSON "module.json"

#include <dlfcn.h>
#include "string.h"
#include "file.h"
#include "engine.h"

typedef struct Dependency {
  const char* name;
  const char* version;
  struct Dependency* next;
} Dependency;

typedef struct Module {
  const char* name;
  const char* description;
  const char* version;
  const char* url;
  const char* license;
  Dependency* dependencies;
  struct Module* next;
} Module;

typedef JSModuleDef* (*Export)(JSContext* context, const char* name);
typedef JSModuleDef* (*Loader)(JSContext* context, const char* path, bool main);

char* flog_module_resolve(JSContext*, char const*, char const*, void*);
JSModuleDef* flog_module_load(JSContext*, const char*, void*);
JSModuleDef* flog_module_load_main(JSContext*, const char*);
JSModuleDef* flog_run_code(JSContext*, char*);
void flog_module_install(Module*, const char*);
#endif
