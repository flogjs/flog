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

#include "resolve.h"

char* flog_dirname(const char* path) {
  int last = flog_string_last(path, '/');
  return flog_string_slice(path, 0, last == -1 ? 0 : last + 1);
}

char* flog_resolve_module(JSContext* context,
                          char const* from,
                          char const* specifier,
                          void* opaque) {
  // absolute path
  if (specifier[0] == '/') {
    char* buffer = flog_string_copy(specifier);
    char* resolved = realpath(buffer, NULL);
    free(buffer);
    return resolved;
  }
  char cwd[PATH_MAX];
  getcwd(cwd, sizeof(cwd));
  flog_log("  [cwd] = %s\n", cwd);
  flog_log("  [from] = %s\n", from);

  char* from_directory = flog_dirname(from);
  flog_log("  [from_directory] = %s\n", from_directory);
  char* directory = flog_string_glue(cwd, from_directory);
  free(from_directory);
  flog_log("  [directory] = %s\n", directory);
  flog_log("  [specifier] = %s\n", specifier);
  char* unresolved = flog_string_glue(directory, specifier);
  free(directory);
  char* resolved = realpath(unresolved, NULL);
  free(unresolved);
  flog_log("  return = %s\n", resolved);
  return resolved;
}
