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
                         ModuleJSON* module_json,
                         JSValueConst json,
                         const char* name) {
  JSValue property = JS_GetPropertyStr(context, json, name);
  if (!JS_IsUndefined(property)) {
    const char* string = JS_ToCString(context, property);
    if (flog_string_equals(name, "name")) {
      module_json->name = string;
    }
    if (flog_string_equals(name, "description")) {
      module_json->description = string;
    }
    if (flog_string_equals(name, "version")) {
      module_json->version = string;
    }
    if (flog_string_equals(name, "url")) {
      module_json->url = string;
    }
    if (flog_string_equals(name, "license")) {
      module_json->license = string;
    }
    JS_FreeCString(context, string);
  }
  JS_FreeValue(context, property);
}

// creates a `module.json` in this directory if doesn't exist
static void init_module_json(const char path[]) {
  RETURN_IF(flog_file_exists(path));
  status("No %s found, initializing", MODULE_JSON);
  ModuleJSON* module_json = calloc(1, sizeof(* module_json));
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  module_json->name = basename(cwd);
  flog_write_module_json(path, module_json);
  free(module_json);
}

static void show_deps(ModuleJSON* module_json) {
  if (module_json != NULL && module_json->dependencies != NULL)  {
    Dependency* dep = module_json->dependencies;
    while (dep != NULL) {
      printf("dependency: %s\n", dep->name);
      dep = dep->next;
    }
  }
}




ModuleJSON* flog_read_module_json(const char path[]) {
  JSContext* context = flog_context();
  init_module_json(path);
  ModuleJSON* module_json = calloc(1, sizeof(* module_json));
  char* code = flog_file_read(path);
  size_t code_length = flog_string_length(code);

  if (code != NULL) {
    JSValue json = JS_ParseJSON(context, code, code_length, MODULE_JSON);
    if (JS_IsObject(json)) {
      set_property(context, module_json, json, "name");
      set_property(context, module_json, json, "description");
      set_property(context, module_json, json, "version");
      set_property(context, module_json, json, "url");
      set_property(context, module_json, json, "license");

      JSAtom deps_atom = JS_NewAtom(context, "dependencies");
      JSValue dependencies = JS_GetProperty(context, json, deps_atom);
      if (JS_IsObject(dependencies)) {
        uint32_t length;
        JSPropertyEnum *tab;
        JSValue js_value;
        const char* property, * value;

        JS_GetOwnPropertyNames(context, &tab, &length, dependencies,
                                 JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY);
        for(size_t i = 0; i < length; i++) {
          property = JS_AtomToCString(context, tab[i].atom);
          js_value = JS_GetProperty(context, dependencies, tab[i].atom);
          value = JS_ToCString(context, js_value);
          flog_add_dependency(module_json, property, value);
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
    printf("name: %s\n", module_json->name);
    printf("license: %s\n", module_json->license);
  }
  show_deps(module_json);
  free(code);
  return module_json;
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

void flog_add_dependency(ModuleJSON* module_json,
                         const char name[],
                         const char version[]) {
  if (!dependency_missing(module_json->dependencies, name)) {
    Dependency* dependency = module_json->dependencies;
    Dependency* new_dependency = calloc(1, sizeof(* new_dependency));
    new_dependency->name = name;
    new_dependency->version = version;
    if (dependency == NULL) {
      module_json->dependencies = new_dependency;
    } else {
      dependency->next = new_dependency;
    }
  }
}

void flog_write_module_json(const char path[], ModuleJSON* module_json) {
  JSContext* context = flog_context();

  JSValue json = JS_NewObject(context);
  SET_STRING(json, "name", module_json->name);
  JSValue space = JS_NewInt32(context, 2);
  JSValue jsonstr = JS_JSONStringify(context, json, JS_UNDEFINED, space);

  const char* string = JS_ToCString(context, jsonstr);
  flog_write_file(path, string);
  JS_FreeCString(context, string);

  JS_FreeValue(context, jsonstr);
  JS_FreeValue(context, json);
}
