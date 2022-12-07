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
void flog_module_install(const char*);
#endif
