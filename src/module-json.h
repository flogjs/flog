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

#ifndef FLOG_MODULE_JSON_H_
#define FLOG_MODULE_JSON_H_

typedef struct Dependency {
  const char* name;
  const char* version;
  struct Dependency* next;
} Dependency;

typedef struct ModuleJSON {
  const char* name;
  const char* description;
  const char* version;
  const char* url;
  const char* license;
  Dependency* dependencies;
} ModuleJSON;

ModuleJSON* flog_read_module_json(const char []);
void flog_add_dependency(ModuleJSON*, const char [], const char []);
void flog_write_module_json(const char [], ModuleJSON*);

#endif
