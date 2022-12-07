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
#include "string.h"

char* flog_dirname(const char* path) {
  int last = flog_string_last(path, '/');
  return flog_string_slice(path, 0, last == -1 ? 0 : last + 1);
}

static size_t get_length(char const string[]) {
  size_t length = 0;
  while (string[length] != 0) {
    length++;
  }
  return length;
}
size_t (* flog_string_length)(char const string[]) = get_length;

int flog_string_index_of(char const string[], const char c) {
  int i = 0;
  char s = 0;
  while ((s = string[i]) != 0) {
    if (s == c) {
      return i;
    }
    i++;
  }
  return -1;
}

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
