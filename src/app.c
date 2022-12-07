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
#include "app.h"

static App* app = 0;

void flog_app_new() {
  if (app == 0) {
    app = calloc(1, sizeof(* app));
    app->engine = flog_engine_new();
    app->database = flog_database_new();
  };
}

void flog_app_free() {
  if (app != 0) {
    flog_engine_free(app->engine);
    flog_database_free(app->database);
    free(app);
  }
}

JSContext* flog_context() {
  return app->engine->context;
}

Database* flog_database() {
  return app->database;
}

void flog_add_module(Module* mod) {
  if (app->modules == 0) { // no modules yet
    mod->next = mod;
    app->modules = mod;
  } else { // cycle though
    mod->next = app->modules;
    app->modules = mod;
  }
}
