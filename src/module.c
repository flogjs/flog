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

#include "module.h"
#include "string.h"
#include "flog.h"

void show_error(JSContext* context) {
  JSValue exception = JS_GetException(context);
  int is_error = JS_IsError(context, exception);
  if (is_error) {
    JSValue message = JS_GetPropertyStr(context, exception, "message");
    if (!JS_IsUndefined(message)) {
      const char* str = JS_ToCString(context, message);
      printf("%s\n", str);
      JS_FreeCString(context, str);
    }
    JS_FreeValue(context, message);
    JSValue stack = JS_GetPropertyStr(context, exception, "stack");
    if (!JS_IsUndefined(stack)) {
      const char* str = JS_ToCString(context, stack);
      printf("%s\n", str);
      JS_FreeCString(context, str);
    }
    JS_FreeValue(context, stack);
  }
  JS_FreeValue(context, exception);
}

static JSModuleDef* create_module(JSContext* context,
                                  DynBuf* dynbuf,
                                  const char* path,
                                  bool main) {
  dbuf_putc(dynbuf, '\0');

  char* buffer = (char*) dynbuf->buf;
  size_t size = dynbuf->size - 1;
  int flags = JS_EVAL_TYPE_MODULE | JS_EVAL_FLAG_COMPILE_ONLY;
  JSValue value = JS_Eval(context, buffer, size, path, flags);
  dbuf_free(dynbuf);

  if (!JS_IsException(value)) {
    if (main) {
      value = JS_EvalFunction(context, value);
      if (JS_IsException(value)) {
        show_error(context);
      }
    }
  } else {
    show_error(context);
    JS_FreeValue(context, value);
    return NULL;
  }

  JSModuleDef* module_def = JS_VALUE_GET_PTR(value);
  JS_FreeValue(context, value);

  return module_def;
}

JSModuleDef* flog_run_code(JSContext* context, char* code) {
  if (code == NULL) {
    flog_log("flog_run_code: no code passed");
    return NULL;
  }

  DynBuf dynbuf;
  dbuf_init(&dynbuf);

  for (const char *i = code; *i != '\0'; i++) {
    dbuf_putc(&dynbuf, *i);
  }

  return create_module(context, &dynbuf, "eval", true);
}

/* Load a module from a JavaScript (.js) file. */
static JSModuleDef* load_js(JSContext* context, const char* path, bool main) {
  flog_log("static load_js: %s\n", path);

  char* code = flog_file_read(path);
  if (code == NULL) {
    flog_log("Cannot find path: %s\n", path);
    return NULL;
  }

  DynBuf dynbuf;
  dbuf_init(&dynbuf);

  for (const char *i = code; *i != '\0'; i++) {
    dbuf_putc(&dynbuf, *i);
  }
  free(code);

  return create_module(context, &dynbuf, path, main);
}

/* Load a module from a JavaScript Object Notation (.json) file. */
static JSModuleDef* load_json(JSContext* context, const char* path, bool main) {
  flog_log("static load_json\n");
  static const char pre[] = "export default JSON.parse(`";
  static const char post[] = "`);";
  
  DynBuf dynbuf;
  dbuf_init(&dynbuf);
  
  dbuf_put(&dynbuf, (const uint8_t *) pre, flog_string_length(pre));

  char* code = flog_file_read(path);
  for (const char *i = code; *i != '\0'; i++) {
    dbuf_putc(&dynbuf, *i);
  }
  free(code);

  dbuf_put(&dynbuf, (const uint8_t *) post, flog_string_length(post));

  return create_module(context, &dynbuf, path, false);
}

/* Load a module from a shared object (.so) file. */
static JSModuleDef* load_so(JSContext* context, const char* path, bool main) {
  flog_log("static load_so\n");
  void* lib = dlopen(path, RTLD_LAZY);
  if (!lib) {
    flog_string_print("error: %s\n", dlerror());
    goto fail;
  }

  Export export = dlsym(lib, "flog_init_module");
  if (!export) {
    fail:
      if (lib) {
        dlclose(lib);
      }
      return NULL;
  }

  return export(context, path);
}

/*
 * Load a module from the library. If the module exists in the library, it will
 * resolve to an actual file under modules/<module>/<version>/main.js
 
 * If the module hasn't been synced yet to the local project, flog will sync
 * the current version in the database to the modules directory.
 */
static JSModuleDef* load_lib(JSContext* context, const char* path, bool main) {
  Database* database = flog_database();
  //if (!database->has((char*) path)) {
//    char* copy = flog_string_copy(name);
  //}
  return 0;
  //return load_js(context, database->get_path((char*) path), false);
}

static Loader resolve(char const* path) {
  if (flog_string_endswith(path, ".js")) {
    return load_js;
  }
  if (flog_string_endswith(path, ".json")) {
    return load_json;
  }
  if (flog_string_endswith(path, ".so")) {
    return load_so;
  }

  return load_lib;
}

JSModuleDef* flog_module_load(JSContext* context,
                              const char* path,
                              void* opaque) {
  flog_log("flog_module_load: %s\n", path);
  return resolve(path)(context, path, false);
}

JSModuleDef* flog_module_load_main(JSContext* context, const char* path) {
  flog_log("flog_module_load: %s\n", path);
  return resolve(path)(context, path, true);
}

char* flog_module_resolve(JSContext* context,
                          char const* from,
                          char const* specifier,
                          void* opaque) {
  // absolute path
  if (specifier[0] == '/') {
    char* buffer = flog_string_copy(specifier);
    char* resolved = realpath(buffer, NULL);
    free(buffer);
    return resolved;
  }
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  flog_log("  [cwd] = %s\n", cwd);
  flog_log("  [from] = %s\n", from);

  char* from_directory = flog_dirname(from);
  flog_log("  [from_directory] = %s\n", from_directory);
  char* directory;
  if (flog_string_length(from_directory) == 0) {
    // from is not qualified, attach to cwd
    directory = flog_string_glue(cwd, from_directory);
    free(from_directory);
  } else {
    // from is already qualified
    directory = from_directory;
  }
  flog_log("  [directory] = %s\n", directory);
  flog_log("  [specifier] = %s\n", specifier);
  char* unresolved = flog_string_glue(directory, specifier);
  free(directory);
  char* resolved = realpath(unresolved, NULL);
  free(unresolved);
  flog_log("  return = %s\n\n", resolved);
  return resolved;
}

static void init_modules_directory() {
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  char* modules = flog_string_glue(cwd, "modules");
  CLEAN_IF(flog_file_exists(modules));

  info("Creating modules directory");
  mkdir(modules, 0744);

  clean:
    free(modules);
}

static void init_namespace_directory(const char module[]) {
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  char* modules = flog_string_glue(cwd, "modules");
  int index = flog_string_index_of(module, '/');
  char* namespace = flog_string_slice(module, 0, index);
  char* namespace_directory = flog_string_glue(modules, namespace);
  CLEAN_IF(flog_file_exists(namespace_directory));

  mkdir(namespace_directory, 0744);

  clean:
    free(namespace);
    free(namespace_directory);
}

void flog_module_install(Module* module, const char* name) {
  Database* database = flog_database();

  info("Querying database");
  if (!flog_database_has(database, name)) {
    error(" target not found", name);
  } else {
    init_modules_directory();
    info("Installing modules");
    init_namespace_directory(name);
    more(" %s ", name);
    fflush(NULL);
    // actually install module 
    // 1 clone repo if necessary / make sure it's up-to-date
    // 2 create a local checkout in modules
    more("%s✓%s\n", COLOR_GREEN, OFF);
  }
}
