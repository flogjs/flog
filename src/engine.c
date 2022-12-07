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
#include "engine.h"
#include "module.h"

static JSValue print(JSContext* context,
                     JSValueConst target, 
                     int argc,
                     JSValueConst* argv) {
  const char* str = JS_ToCString(context, *argv);
  printf("%s\n", str);
  return JS_UNDEFINED;
}

Engine* flog_engine_new() {
  Engine* engine = calloc(1, sizeof(* engine));
  engine->runtime = JS_NewRuntime();
  engine->context = JS_NewContext(engine->runtime);

  /* module load function */
  JS_SetModuleLoaderFunc(engine->runtime, flog_module_resolve,
    flog_module_load, engine);
  JSValue global_obj = JS_GetGlobalObject(engine->context);
  JSAtom ns_atom = JS_NewAtom(engine->context, "log");
  JSValue obj = JS_NewCFunction(engine->context, print, "log", 0);
  //JSValue ns = JS_NewObjectProto(engine->context, JS_NULL);
  //JS_DupValue(engine->context, obj);
  JS_SetProperty(engine->context, global_obj, ns_atom, obj);

  JS_FreeAtom(engine->context, ns_atom);
//  JS_FreeValue(engine->context, obj);

  JS_FreeValue(engine->context, global_obj);

  return engine;
}

void flog_engine_free(Engine* engine) {
  JS_FreeContext(engine->context);
  JS_FreeRuntime(engine->runtime);
  free(engine);
}
