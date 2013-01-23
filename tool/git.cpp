/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <git2.h>
#include <git2/remote.h>

#include "git.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <string>

namespace
{

struct git_failure: std::runtime_error
  {
  git_failure() : std::runtime_error(giterr_last()->message)
    {
    }
  };

static inline void check_git_failure(int error)
  {
  if (error)
    {
    throw git_failure();
    }
  }

template<typename T>
struct at_scope_exit
  {
  public:
    at_scope_exit(T* self, void (*func)(T*)) :
        self(self), func(func)
      {
      }
    ~at_scope_exit()
      {
      func(self);
      }
  private:
    T* self;
    void (*func)(T*);
  };

#if defined(_MSC_VER) || defined(__MINGW32__)
#  define PRIuZ "Iu"
#else
#  define PRIuZ "zu"
#endif

struct progress_data
  {
  git_transfer_progress fetch_progress;
  size_t completed_steps;
  size_t total_steps;
  const char *path;
  };

static void print_progress(const progress_data *pd)
  {
  int network_percent = (100*pd->fetch_progress.received_objects) / pd->fetch_progress.total_objects;
  int index_percent = (100*pd->fetch_progress.indexed_objects) / pd->fetch_progress.total_objects;
  int checkout_percent = (int)(pd->total_steps > 0 ? (100 * pd->completed_steps) / pd->total_steps : 0.f);
  int kbytes = pd->fetch_progress.received_bytes / 1024;

  printf("net %3d%% (%4d kb, %5d/%5d)  /  idx %3d%% (%5d/%5d)  /  chk %3d%% (%4" PRIuZ "/%4" PRIuZ ") %s\r",
      network_percent, kbytes,
      pd->fetch_progress.received_objects, pd->fetch_progress.total_objects,
      index_percent, pd->fetch_progress.indexed_objects, pd->fetch_progress.total_objects,
      checkout_percent,
      pd->completed_steps, pd->total_steps,
      pd->path);
  }

static void fetch_progress(const git_transfer_progress *stats, void *payload)
  {
  progress_data *pd = (progress_data*) payload;
  pd->fetch_progress = *stats;
  print_progress(pd);
  }

static void checkout_progress(const char *path, size_t cur, size_t tot, void *payload)
  {
  progress_data *pd = (progress_data*) payload;
  pd->completed_steps = cur;
  pd->total_steps = tot;
  pd->path = path;
  print_progress(pd);
  }

static int cred_acquire(git_cred **cred, const char* url, unsigned int allowed_types, void*)
  {
  std::string username, password;
  std::cout << "cred required for " << url << std::endl;
  std::cout << "username: ";
  std::cin >> username;
  std::cout << "password: ";
  std::cin >> password;
  return git_cred_userpass_plaintext_new(cred, username.c_str(), password.c_str());
  }

} // namespace

namespace Karrot
{

void Git::download(const Implementation& impl, bool requested)
  {
  const char* href = impl.values.at("href").c_str();
  const char* hash = impl.values.at("tag").c_str();
  const char* path = impl.name.c_str();

  progress_data pd;
  memset(&pd, 0, sizeof(pd));

  git_checkout_opts checkout_opts;
  memset(&checkout_opts, 0, sizeof(checkout_opts));
  checkout_opts.version = GIT_CHECKOUT_OPTS_VERSION;
  checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
  checkout_opts.progress_cb = checkout_progress;
  checkout_opts.progress_payload = &pd;

  git_repository* repo = 0;
  git_remote* origin = 0;
  git_object* object = 0;

  at_scope_exit<git_repository> repository_free(repo, git_repository_free);
  at_scope_exit<git_remote> remote_free(origin, git_remote_free);
  at_scope_exit<git_object> object_free(object, git_object_free);

  if (git_repository_open(&repo, path) == 0)
    {
    check_git_failure(git_remote_load(&origin, repo, "origin"));
    if (std::strcmp(git_remote_url(origin), href) != 0)
      {
      throw std::runtime_error("different origin");
      }
    }
  else
    {
    check_git_failure(git_repository_init(&repo, path, false));
    check_git_failure(git_remote_create(&origin, repo, "origin", href));
    }
  git_remote_set_update_fetchhead(origin, 0);
  git_remote_set_cred_acquire_cb(origin, cred_acquire, 0);
  check_git_failure(git_remote_connect(origin, GIT_DIRECTION_FETCH));
  at_scope_exit<git_remote> remote_disconnect(origin, git_remote_disconnect);
  check_git_failure(git_remote_download(origin, fetch_progress, &pd));
  check_git_failure(git_remote_update_tips(origin));
  check_git_failure(git_revparse_single(&object, repo, hash));
  check_git_failure(git_checkout_tree(repo, object, &checkout_opts));
  }

} // namepsace karrot
