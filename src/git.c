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

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "git.h"

typedef struct Options {
  const char** heads;
  size_t heads_count;

  git_annotated_commit** annotated;
  size_t annotated_count;

  int no_commit : 1;
} Options;

static int fastforward(git_repository* repo, const git_oid* target_oid) {
  git_checkout_options ff_checkout_options = GIT_CHECKOUT_OPTIONS_INIT;
  git_reference* target_ref;
  git_reference* new_target_ref;
  git_object* target = NULL;
  int err = 0;

  /* HEAD exists, just lookup and resolve */
  err = git_repository_head(&target_ref, repo);
  if (err != 0) {
    fprintf(stderr, "failed to get HEAD reference\n");
    return -1;
  }

  /* Lookup the target object */
  err = git_object_lookup(&target, repo, target_oid, GIT_OBJECT_COMMIT);
  if (err != 0) {
    fprintf(stderr, "failed to lookup OID %s\n", git_oid_tostr_s(target_oid));
    return -1;
  }

  /* Checkout the result so the workdir is in the expected state */
  ff_checkout_options.checkout_strategy = GIT_CHECKOUT_SAFE;
  err = git_checkout_tree(repo, target, &ff_checkout_options);
  if (err != 0) {
    fprintf(stderr, "failed to checkout HEAD reference\n");
    return -1;
  }

  /* Move the target reference to the target OID */
  err = git_reference_set_target(&new_target_ref, target_ref, target_oid, NULL);
  if (err != 0) {
    fprintf(stderr, "failed to move HEAD reference\n");
    return -1;
  }

  git_reference_free(target_ref);
  git_reference_free(new_target_ref);
  git_object_free(target);

  return 0;
}

static int fetchhead_cb(const char* ref_name,
                        const char* remote_url,
                        const git_oid* oid,
                        unsigned int is_merge,
                        void* payload) {
  if (is_merge) {
    git_oid_cpy((git_oid*) payload, oid);
  }

  return 0;
}

bool flog_git_pull_master(Database* database) {
  git_oid forward_to;
  git_repository* repo = database->repo;

  git_repository_fetchhead_foreach(repo, fetchhead_cb, &forward_to);

  git_annotated_commit** commit = calloc(1, sizeof(git_annotated_commit*));

  git_annotated_commit_from_fetchhead(commit, repo,
                                      "refs/heads/master",
                                      git_remote_url(database->remote),
                                      &forward_to);

  git_merge_analysis_t analysis;
  git_merge_preference_t preference = GIT_MERGE_PREFERENCE_FASTFORWARD_ONLY;

  int error = git_merge_analysis(&analysis, &preference, repo,
      (const git_annotated_commit**) commit, 1);

  git_annotated_commit_free(commit[0]);

  if (analysis & GIT_MERGE_ANALYSIS_FASTFORWARD) {
    fastforward(repo, (const git_oid*) &forward_to);
    //char* out = calloc(8, sizeof(char*));
    return true;//git_oid_tostr(out, 8, &forward_to);
  } 
  return false;
}

void flog_git_clone(const char* repository, const char* directory) {
  git_repository* repo = NULL;
  int ret = git_clone(&repo, repository, directory, NULL);
  if (ret != 0) {
    error("Couldn't initialise database");
  }
  git_repository_free(repo);
}

void flog_git_open(Database* database, const char* path) {
  int error = git_repository_open(&database->repo, path);
  error = git_remote_lookup(&database->remote, database->repo, "origin");
}

void flog_git_close(Database* database) {
  git_repository_free(database->repo);
  git_remote_free(database->remote);
}

void flog_git_init() {
  git_libgit2_init();
}

void flog_git_free() {
  git_libgit2_shutdown();
}

