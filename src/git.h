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

#ifndef FLOG_GIT_H_
#define FLOG_GIT_H_

#include <git2.h>
#include "flog.h"

void flog_git_init();
void flog_git_free();
void flog_git_clone(const char*, const char*);
void flog_git_clone_bare(const char*, const char*);
bool flog_git_pull_master(Database*);
void flog_git_open(Database*, const char*);
void flog_git_close(Database*);

#endif
