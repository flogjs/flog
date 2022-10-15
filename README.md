# Flog

Flog is a **JavaScript** runtime. Written on top of the QuickJS in C, it has a
minimal core.

## Design goals

* Minimal core as a thin wrapper around QuickJS
* Executeable and module manager in one
* Namespaced, officially supported standard library (`std`)
* Third-party, scoped module area
* Hot code replacement in development mode
* Sandboxing of applications (directory-level scoping)

### Getting started

On Linux, run `make` to compile. Run the resulting `flog` executeable with a
JS file as the first argument.

Currently supported are `.js` and `.so` imports.

### Creating a C module

Define a `flog_init_module` function with the given signature. This is an
example for a module that emulates `console.log`.

```c
#include <stdio.h>
#include "../flog/src/flog.h"

static JSValue print(JSContext* context,
                     JSValueConst target,
                     int argc,
                     JSValueConst* argv) {
  if (argc < 1) return JS_UNDEFINED;
  const char* str = JS_ToCString(context, * argv);
  printf("%s\n", str);
  return JS_UNDEFINED;
}

static const JSCFunctionListEntry funcs[] = {
  JS_CFUNC_DEF("log", 1, print),
};

static int module_init(JSContext* context, JSModuleDef* module_def) {
  JSValue console = JS_NewObjectProto(context, JS_NULL);
  JS_SetPropertyFunctionList(context, console, funcs, countof(funcs));
  return JS_SetModuleExport(context, module_def, "default", console);
}

JSModuleDef* flog_init_module(JSContext* context, const char* name) {
  JSModuleDef* module_def = JS_NewCModule(context, name, module_init);
  if (!module_def) {
    return NULL;
  }

  JS_AddModuleExport(context, module_def, "default");

  return module_def;
}
```

Compile with a `Makefile` that references that original `libqjs.a` artefact
created when having compiled `flog`.

```Makefile
console:
	gcc -c -L../flog -llibqjs -o console.o console.c
	gcc -o console.so -shared -Wl,-soname=console.so -Wl,--start-group console.o -Wl,--end-group
```

Then in your .js file, import and use.

```js
import console from "console.so";

console.log("Hi!");
```

### License

AGPL3.0
