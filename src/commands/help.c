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

#include "./base.h"
#include "../version.h"

void line(char const*, char const*);
void line(char const* operation, char const* explanation) {
  flog_string_print("  flog %-20s %s\n", operation, explanation);
}

int flog_command_help() {
  flog_string_print("flog %s\n", FLOG_VERSION);
  flog_string_print("quickjs %s\n\n", QJS_VERSION);
  flog_string_print("usage:  flog <file>.js\n");
  flog_string_print("        flog <operation> [...]\n\n");
  flog_string_print("local operations (current directory):\n");
  line("sync <module>", "add/update <module> as dependency into module.json");
  line("list", "show synced modules"),
  flog_string_print("\nglobal operations:\n");
  line("info <module>", "show info about <module>");
  line("with <module> [...]", "execute <module> with ... as params");
  return 0;
}
