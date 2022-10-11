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

#include "app.h"

static FlogApp* app = 0;

FlogApp* flog_new_app() {
  if (app == 0) {
    app = calloc(1, sizeof(*app));
    app->engine = flog_engine_new();

    return app;
  };
}

void flog_dispose_app(FlogApp* app2) {
  if (app2 != 0) {
    flog_engine_dispose(app2->engine);
    free(app2);
  }
}

FlogEngine* flog_engine() {
  return app->engine;
}

FlogDatabase* flog_database() {
  return app->database;
}

FlogModule* flog_modules() {
  return app->modules;
}

void flog_add_module(FlogModule* mod) {
  if (app->modules == 0) { // no modules yet
    mod->next = mod;
    app->modules = mod;
  } else { // cycle though
    mod->next = app->modules;
    app->modules = mod;
  }
}
