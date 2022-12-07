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
#include "base.h"
#include "../string.h"
#include "../app.h"
    
// <file>.js, sync, list
static int args2(int argc, char* argv[]) {
  if (argc != 2) {
    return -1;
  }

  char* operation = argv[1];

  if (flog_string_endswith(operation, ".js")) {
    return flog_command_file(operation, argc, argv);
  }

  if (flog_string_equals(operation, "update")) {
    return flog_command_update();
  }

  if (flog_string_equals(operation, "list")) {
    return flog_command_list();
  }

  if (flog_string_equals(operation, "sync")) {
    return flog_command_sync();
  }
    
  if (flog_string_equals(operation, "help") ||
      flog_string_equals(operation, "-h") ||
      flog_string_equals(operation, "--help")) {
    return flog_command_help();
  }

  return 0;
}

// sync, info, with
static int args3(int argc, char* argv[]) {
  if (argc != 3) {
    return -1;
  }

  char* operation = argv[1];
  char* param = argv[2];

  if (flog_string_equals(operation, "-e")) {
    return flog_command_script(param);
  }

  if (flog_string_equals(operation, "install")) {
    return flog_command_install(param);
  }

  if (flog_string_equals(operation, "info")) {
    return flog_command_info(param);
  }

  if (flog_string_equals(operation, "with")) {
    return flog_command_with(param);
  }

  return 0;
}

int flog_command_args(int argc, char* argv[]) {
  flog_app_new();

  int result = argc == 2 ? args2(argc, argv) : args3(argc, argv);

  flog_app_free();

  return result;
}
