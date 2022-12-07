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
#include "module.h"
#include "module-json.h"
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

  Export export = dlsym(lib, "flog_module_init");
  if (!export) {
    fail:
      if (lib) {
        dlclose(lib);
      }
      return NULL;
  }

  return export(context, path);
}

static bool has_local_module(char const* target) {
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  char modules[PATH_MAX+9];
  sprintf(modules, "%s/modules/%s", cwd, target);

  return flog_file_exists(modules);
}
/*
 * Load a module from the library. If the module exists in the library, it will
 * resolve to an actual file under modules/<module>/<version>/main.js
 
 * If the module hasn't been synced yet to the local project, flog will sync
 * the current version in the database to the modules directory. It will do so
 * automatically for modules in the `std` namespace, but will ask for other
 * modules.
 */
static JSModuleDef* load_lib(JSContext* context, const char* target, bool _) {
  Database* database = flog_database();
  if (!flog_database_has(database, target)) {
    error(" module `%s` not found", target);
    return NULL;
  }
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  char entry[PATH_MAX+17];
  sprintf(entry, "%s/modules/%s/main.js", cwd, target);

  if (has_local_module(target)) {
    // load its main file
    return load_js(context, entry, false);
  } else {
    // if std, install automatically
    if (target[0] == 's' && target[1] == 't' && target[2] == 'd'
        && target[3] == '/') {
      flog_module_install(target);

      printf("\n");
    
      return load_js(context, entry, false);
    }
    // ask if would like to install
  }
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
  return resolve(path)(context, path, false);
}

JSModuleDef* flog_module_load_main(JSContext* context, const char* path) {
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
  // library module
  if (flog_string_index_of(specifier, '.') == -1) {
    char* buffer = flog_string_copy(specifier);
    return buffer;
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

static void init_modules_directory(const char path[]) {
  RETURN_IF(flog_file_exists(path));
  mkdir(path, 0744);
  info("Creating modules directory");
}

static void init_directory(const char directory[]) {
  RETURN_IF(flog_file_exists(directory));
  mkdir(directory, 0744);
}

static void init_directories(const char module[]) {
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  char* modules = flog_string_glue(cwd, "modules");

  // ensure `modules` exists
  init_modules_directory(modules);

  // ensure `modules/${namespace}` exists
  int index = flog_string_index_of(module, '/');
  char* namespace = flog_string_slice(module, 0, index);
  char* namespace_directory = flog_string_glue(modules, namespace);
  init_directory(namespace_directory);
  free(namespace_directory);
  free(namespace);

  // ensure `modules/${namespace}/${module}` exists
  //char* module_directory = flog_string_glue(modules, module);
  //init_directory(module_directory);
  //free(module_directory);

  free(modules);
}

void flog_module_install(const char* target) {
  // make sure module.json exists
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  char* path = flog_string_glue(cwd, MODULE_JSON);
  Module* module = flog_module_json_read(path);
  free(path);
  free(module);

  Database* database = flog_database();

  info("Querying database");
  if (!flog_database_has(database, target)) {
    error(" target `%s` not found", target);
  } else {
    init_directories(target);
    more(" %s ", target);
    fflush(NULL);
    // actually install module 
    // 1 clone repo if necessary / make sure it's up-to-date
    flog_database_clone_module(database, target);
    // 2 create a local checkout in modules
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    char module_directory[PATH_MAX+9];
    sprintf(module_directory, "%s/modules/%s", cwd, target);

    flog_database_checkout_module(database, target, module_directory);
    more("%s✓%s\n", COLOR_GREEN, OFF);
  }
}
