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
#include "./base.h"
#include "../version.h"

void line(char const*, char const*);
void line(char const* operation, char const* explanation) {
  flog_string_print("  flog %-20s %s\n", operation, explanation);
}

int flog_command_help() {
  flog_string_print("flog %s\n", FLOG_VERSION);
  flog_string_print("s4 %s\n\n", "2021-03-27");
  flog_string_print(
      "usage:  flog <file>.js              parse and execute <file>.js\n");
  flog_string_print("        flog <operation> [...]      execute operation\n");
  flog_string_print("        flog help                   show this help\n\n");
  flog_string_print("local operations (current directory):\n");
  line("sync <module>", "add/update <module> as dependency into module.json");
  line("list", "show synced modules");
  flog_string_print("\nglobal operations:\n");
  line("info <module>", "show info about <module>");
  line("with <module> [...]", "execute <module> with ... as params");
  return 0;
}
