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
  if (file == NULL) {
    return 0;
  }
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
  if (file == NULL) {
    return false;
  }
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
