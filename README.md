# Flog

Flog is a **JavaScript** runtime. Written on top of QuickJS in C, it has a
minimal core.

## Design goals

* Minimal core as a thin wrapper around QuickJS ✓
* Executable and module manager in one WIP
* Namespaced, officially supported standard library ([flogjs/std][std])
* Third-party, scoped module area TODO
* Sandboxing of applications (directory-level scoping) TODO

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
import console from "std/console";

console.log("Hello, world!");
```

You can now run flog with this file as the first argument.

```sh
./flog app.js
```

Flog will download and install the standard module `console` and will execute
the file.

Standard modules are automatically installed the first time they are used in a
file. In the case of non-standard (third-party) modules encountered in a file,
you will be prompted if you wish to download and install them.

If you wish to explicitly install modules before use (both standard and
third-party), run `flog install [module]`.

```sh
./flog install std/console

```

In addition to normal JavaScript syntax, you can use `import` and `export`
declarations to import and export code via modules. Currently supported are
`.js`, `.json` and `.so` imports.

### Creating a C module

Define a `flog_module_init` function with the given signature. This is an
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

### Resources

* IRC: Join the `#flog` channel on `irc.libera.chat`.

### License

MIT

[rfcs]: https://github.com/flogjs/rfcs
[std]: https://github.com/flogjs/std
