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

#include "file.h"

char* flog_file_read(const char* path) {
  // read file into buffer
  FILE* file = fopen(path, "r");
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* buffer = (char*) malloc(sizeof(char) * (length + 1));
  buffer[length] = 0;
  fread(buffer, sizeof(char), length, file);
  fclose(file);

  return buffer;
}

int flog_file_new_directory(const char* path) {
  return mkdir(path, S_IRWXU);
}
