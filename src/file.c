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

bool flog_file_exists(const char* file) {
  struct stat st = {0};
  return stat(file, &st) == 0;
}

char* flog_file_read(const char* path) {
  // read file into buffer
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* buffer = (char*) malloc(sizeof(char) * (length + 1));
  buffer[length] = 0;
  fread(buffer, sizeof(char), length, file);
  fclose(file);

  return buffer;
}

size_t flog_file_n_lines(const char* path) {
  FILE* file = fopen(path, "r");
  size_t lines = 0;
  if (file == NULL) {
    return lines;
  }
  while (!feof(file)) {
    if (fgetc(file) == '\n') {
      lines++;
    }
  }
  fclose(file);
  return lines; 
}

bool flog_file_match_module_line(const char* path, const char module[]) {
  FILE* file = fopen(path, "r");
  int position = 0;
  int line_length = 80;
  char line[line_length];

  while (fgets(line, line_length, file)) {
    while (module[position] != 0)  {
      if (line[position] != module[position]) {
        position = 0;
        // violation, move to next line
        break;
      }

      // partial match, next character in line must be : for a full match
      if (line[position+1] == ':' && module[position+1] == 0) {
        fclose(file);
        return true;
      }

      position++;
    }
  }

  fclose(file);
  return false;
}

void flog_write_file(const char path[], const char data[]) {
  FILE* file = fopen(path, "w");
  if (file == NULL) {
    return;
  }
  fprintf(file, "%s\n", data);
  fclose(file);
}

int flog_file_new_directory(const char* path) {
  return mkdir(path, S_IRWXU);
}
