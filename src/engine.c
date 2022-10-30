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

Engine* flog_init_engine() {
  Engine* engine = calloc(1, sizeof(* engine));
  engine->runtime = JS_NewRuntime();
  engine->context = JS_NewContext(engine->runtime);

  /* module load function */
  JS_SetModuleLoaderFunc(engine->runtime, flog_resolve_module,
    flog_load_module, engine);
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

void flog_teardown_engine(Engine* engine) {
  JS_FreeContext(engine->context);
  JS_FreeRuntime(engine->runtime);
  free(engine);
}
