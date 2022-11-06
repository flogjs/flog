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
