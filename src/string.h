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
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

#ifndef INCLUDE_FLOG_STRING_H_
#define INCLUDE_FLOG_STRING_H_
char* flog_dirname(const char*);
bool flog_string_endswith(char const search[], char const suffix[]);
bool flog_string_equals(char const left[], char const right[]);
void flog_string_print(char const format[], ...);
extern size_t (* flog_string_length)(char const string[]);
char* flog_string_slice(char const string[], int start, int end);
char* flog_string_copy(const char* string);
char* flog_string_copy2(const char* string, int length);
char* flog_string_format(const char* format, ...);
int flog_string_last(char const string[], char character);
char* flog_string_glue(char const left[], char const right[]);
int flog_string_index_of(char const [], const char);
#endif
