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

static App* app = 0;

void flog_init_app() {
  if (app == 0) {
    app = calloc(1, sizeof(*app));
    app->engine = flog_init_engine();
    app->database = flog_init_database();
  };
}

void flog_teardown_app() {
  if (app != 0) {
    flog_teardown_engine(app->engine);
    flog_teardown_database(app->database);
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
