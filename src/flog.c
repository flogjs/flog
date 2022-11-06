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

#include "flog.h"

void info(const char* format, ...) {
  va_list args;
  va_start(args, format);
  printf(COLOR_BLUE ":: " OFF COLOR_BOLD);
  vprintf(format, args);
  printf(OFF "\n");
  va_end(args);
}

void more(const char* format, ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

void error(const char* format, ...) {
  va_list args;
  va_start(args, format);
  printf(COLOR_RED " error" OFF);
  vprintf(format, args);
  printf(OFF "\n");
  va_end(args);
}

int main(int argc, char** argv) {
  return argc == 1 ? flog_command_help() : flog_command_args(argc, argv);
}
