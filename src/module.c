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
#include "resolve.h"
#include "app.h"

void show_error(JSContext* context) {
  JSValue exception = JS_GetException(context);
  int is_error = JS_IsError(context, exception);
  if (is_error) {
    JSValue value = JS_GetPropertyStr(context, exception, "stack");
    if (!JS_IsUndefined(value)) {
      const char* str = JS_ToCString(context, value);
      printf("%s\n", str);
      JS_FreeCString(context, str);
    }
    JS_FreeValue(context, value);
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
    }
  } else {
    show_error(context);
    JS_FreeValue(context, value);
    return NULL;
  }

  const char* str = JS_ToCString(context, value);
  if (!str) {
    show_error(context);
  }

  return JS_VALUE_GET_PTR(value);

}

/* Load a module from a JavaScript (.js) file. */
static JSModuleDef* load_js(const char* path, JSContext* context, bool main) {
  flog_log("static load_js\n");

  DynBuf dynbuf;
  dbuf_init(&dynbuf);

  char* code = flog_file_read(path);
  for (const char *i = code; *i != '\0'; i++) {
    dbuf_putc(&dynbuf, *i);
  }
  free(code);

  return create_module(context, &dynbuf, path, main);
}

/* Load a module from a JavaScript Object Notation (.json) file. */
static JSModuleDef* load_json(const char* path, JSContext* context, bool main) {
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

/* Load a module from a shared object (.so) file.  */
static JSModuleDef* load_so(const char* path, JSContext* context, bool main) {
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
static JSModuleDef* load_lib(const char* name, JSContext* context, bool main) {
  FlogDatabase* database = flog_database();
  if (!database->has((char*) name)) {
//    char* copy = flog_string_copy(name);
  }
  return load_js(database->get_path((char*) name), context, false);
}

loader resolve(char const* path) {
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
  return resolve(path)(path, context, false);
}

JSModuleDef* flog_module_init(JSContext* context, const char* path) {
  flog_log("flog_module_load: %s\n", path);
  return resolve(path)(path, context, true);
}
