# Flog

Flog is a **JavaScript** runtime. Written on top of the QuickJS in C, it has a
minimal core.

## Design goals

* Minimal core as a thin wrapper around QuickJS ✓
* Executeable and module manager in one
* Namespaced, officially supported standard library (`std`)
* Third-party, scoped module area
* Hot code replacement in development mode
* Sandboxing of applications (directory-level scoping)

### Prerequisites

Your system needs to provide libgit2 as a library. Most Linux distributions
package it.

#### Arch Linux

`pacman -S libgit2`

#### Debian/Ubuntu

`apt-get install libgit2-dev`

### Getting started

On Linux, run `make` to compile. This will result in a `flog` executable in the
same directory.

Create an `app.js` file in the same directory.

```js
log("Hello, world!");
```

You can now run flog with this file as the first argument.

```sh
./flog app.js
```

Besides the standard JavaScript language features introduced by QuickJS, flog
currently has a utility function `log` in the global namespace that emulates
`console.log`. This will be later removed in official releases.

In addition to normal JavaScript syntax, you can use `import` and `export`
declarations to import and export code via modules. Currently supported are
`.js`, `.json` and `.so` imports.

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

JSModuleDef* flog_module_init(JSContext* context, const char* name) {
  JSModuleDef* module_def = JS_NewCModule(context, name, module_init);
  if (!module_def) {
    return NULL;
  }

  JS_AddModuleExport(context, module_def, "default");

  return module_def;
}
```

Compile with a `Makefile` that references the original `libqjs.a` artefact
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
