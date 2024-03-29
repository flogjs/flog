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

void setup_debug_handlers();

int main(int argc, char** argv) {
  setup_debug_handlers();
  return argc == 1 ? flog_command_help() : flog_command_args(argc, argv);
}
