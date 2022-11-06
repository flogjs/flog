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

#include "module-json.h"
#include "flog.h"

#include "file.h"
#include "string.h"
#include "../deps/quickjs/quickjs.h"

static void set_property(JSContext* context,
                         Module* module,
                         JSValueConst json,
                         const char* name) {
  JSValue property = JS_GetPropertyStr(context, json, name);
  if (!JS_IsUndefined(property)) {
    const char* string = JS_ToCString(context, property);
    if (flog_string_equals(name, "name")) {
      module->name = string;
    }
    if (flog_string_equals(name, "description")) {
      module->description = string;
    }
    if (flog_string_equals(name, "version")) {
      module->version = string;
    }
    if (flog_string_equals(name, "url")) {
      module->url = string;
    }
    if (flog_string_equals(name, "license")) {
      module->license = string;
    }
    JS_FreeCString(context, string);
  }
  JS_FreeValue(context, property);
}

// creates a `module.json` in this directory if doesn't exist
static void init_module_json(const char path[]) {
  RETURN_IF(flog_file_exists(path));

  info("Initializing %s", MODULE_JSON);
  Module* module = calloc(1, sizeof(* module));
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  module->name = basename(cwd);
  flog_module_json_write(path, module);
  free(module);
}

static void show_deps(Module* module) {
  if (module != NULL && module->dependencies != NULL)  {
    Dependency* dep = module->dependencies;
    while (dep != NULL) {
      printf("dependency: %s\n", dep->name);
      dep = dep->next;
    }
  }
}

Module* flog_module_json_read(const char path[]) {
  JSContext* context = flog_context();
  init_module_json(path);
  Module* module = calloc(1, sizeof(* module));
  char* code = flog_file_read(path);
  size_t code_length = flog_string_length(code);

  if (code != NULL) {
    JSValue json = JS_ParseJSON(context, code, code_length, MODULE_JSON);
    if (JS_IsObject(json)) {
      set_property(context, module, json, "name");
      set_property(context, module, json, "description");
      set_property(context, module, json, "version");
      set_property(context, module, json, "url");
      set_property(context, module, json, "license");

      JSAtom deps_atom = JS_NewAtom(context, "dependencies");
      JSValue dependencies = JS_GetProperty(context, json, deps_atom);
      if (JS_IsObject(dependencies)) {
        uint32_t length;
        JSPropertyEnum *tab;
        JSValue js_value;
        const char* property, * value;

        JS_GetOwnPropertyNames(context, &tab, &length, dependencies,
                                 JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY);
        for (size_t i = 0; i < length; i++) {
          property = JS_AtomToCString(context, tab[i].atom);
          js_value = JS_GetProperty(context, dependencies, tab[i].atom);
          value = JS_ToCString(context, js_value);
          flog_add_dependency(module, property, value);
          JS_FreeCString(context, value);
          JS_FreeValue(context, js_value);
          JS_FreeCString(context, property);
        }
        printf("IS Object\n");
        js_free(context, tab);
      }
      JS_FreeAtom(context, deps_atom);
      JS_FreeValue(context, dependencies);

    }
    JS_FreeValue(context, json);
    //printf("name: %s\n", module->name);
    //printf("license: %s\n", module->license);
  }
  show_deps(module);
  free(code);
  return module;
}

static bool dependency_missing(Dependency* deps, const char name[]) {
  Dependency* dependency = deps;
  bool found = false;
  while (dependency != NULL) {
    if (flog_string_equals(dependency->name, name)) {
      found = true;
      break;
    }
    dependency = dependency->next;
  }
  return found;
}

void flog_add_dependency(Module* module,
                         const char name[],
                         const char version[]) {
  if (!dependency_missing(module->dependencies, name)) {
    Dependency* dependency = module->dependencies;
    Dependency* new_dependency = calloc(1, sizeof(* new_dependency));
    new_dependency->name = name;
    new_dependency->version = version;
    if (dependency == NULL) {
      module->dependencies = new_dependency;
    } else {
      dependency->next = new_dependency;
    }
  }
}

void flog_module_json_write(const char path[], Module* module) {
  JSContext* context = flog_context();

  JSValue json = JS_NewObject(context);
  SET_STRING(json, "name", module->name);
  JSValue space = JS_NewInt32(context, 2);
  JSValue jsonstr = JS_JSONStringify(context, json, JS_UNDEFINED, space);

  const char* string = JS_ToCString(context, jsonstr);
  flog_write_file(path, string);
  JS_FreeCString(context, string);

  JS_FreeValue(context, jsonstr);
  JS_FreeValue(context, json);
}
