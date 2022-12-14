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
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "git.h"

typedef struct Options {
  const char** heads;
  size_t heads_count;

  git_annotated_commit** annotated;
  size_t annotated_count;

  int no_commit: 1;
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

void flog_git_clone_bare(const char* repository, const char* directory) {
  git_repository* repo = NULL;
  git_clone_options options = GIT_CLONE_OPTIONS_INIT;
  options.bare = 1;
  int ret = git_clone(&repo, repository, directory, &options);
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

