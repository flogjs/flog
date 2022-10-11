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

#include "string.h"

static size_t get_length(char const string[]) {
  size_t length = 0;
  while (string[length] != 0) {
    length++;
  }
  return length;
}
size_t (*flog_string_length)(char const string[]) = get_length;

bool flog_string_endswith(char const search[], char const ending[]) {
  size_t search_length = get_length(search);
  size_t ending_length = get_length(ending);

  if (search_length < ending_length) {
    flog_log("endswith error: search_length smaller than ending_length");
    return 0;
  }

  for (size_t i = 0; i < ending_length; i++) {
    if (search[search_length-ending_length+i] != ending[i])  {
      return 0;
    }
  }

  return 1;
}

bool flog_string_equals(char const left[], char const right[]) {
  if (left == 0 || right == 0) {
    return 0;
  }

  if (get_length(left) != get_length(right)) {
    return 0;
  }

  for (size_t i = 0; left[i] != 0; i++) {
    if (left[i] != right[i]) {
      return 0;
    }
  }

  return 1;
}

void flog_string_print(char const format[], ...) {
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

char* flog_string_slice(char const string[], int start, int end) {
  size_t length = end - start;
  char* buffer = (char*) malloc(sizeof(char) * (length+1));
  buffer[length] = 0;
  for (int i = start; i < end; i++) {
    buffer[i - start] = string[i];
  }
  return buffer;
}

char* flog_string_copy(const char* string) {
  size_t length = 0;
  while (string[length] != 0) {
    length++;
  }
  return flog_string_slice(string, 0, length);
}

char* flog_string_copy2(const char* string, int length) {
  return flog_string_slice(string, 0, length);
}

char* flog_string_format(const char* format, ...) {
  char buffer[1024]; /* path max */
  va_list args;
  va_start(args, format);
  int length = vsprintf(buffer, format, args);
  va_end(args);
  return flog_string_copy2(buffer, length);
}

int flog_string_last(char const string[], char const character) {
  int position = -1;
  for (size_t i = 0; string[i] != 0; i++) {
    if (string[i] == character) {
      position = i;
    }
  }
  return position;
}

char* flog_string_glue(char const left[], char const right[]) {
  size_t left_length = get_length(left);
  size_t right_length = get_length(right);
  size_t total = left_length + right_length + 1;
  char* buffer = (char*) malloc(sizeof(char) * (total+1));
  buffer[total] = 0;

  for (size_t i = 0; i < left_length; i++) {
    buffer[i] = left[i];
  }
  buffer[left_length] = '/';
  for (size_t i = 0; i < right_length; i++) {
    buffer[left_length+1+i] = right[i];
  }

  return buffer;
};
