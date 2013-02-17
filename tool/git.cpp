/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <git2.h>
#include <git2/remote.h>
#include <boost/throw_exception.hpp>

#include "git.hpp"

#include <cassert>
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

#define CALL_GIT(FUNCTION, ...)                                                \
  if (FUNCTION(__VA_ARGS__))                                                   \
    {                                                                          \
    BOOST_THROW_EXCEPTION(git_failure());                                      \
    }                                                                          \
  void(0)                                                                      \

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

class Progress
  {
  public:
    Progress(const char *name)
        : name{name}, steps{0, 0, 0}
      {
      }
    ~Progress()
      {
      printf("\n");
      }
    void update(int step, std::size_t completed, std::size_t total)
      {
      assert(step < 3);
      steps[step] = total > 0 ? (60 * completed) / total : 0;
      }
    void print()
      {
      int i = 1;
      printf("%-16s[", name);
      for (; i <= steps[0]; ++i)
        {
        printf("#");
        }
      for (; i <= steps[1]; ++i)
        {
        printf("=");
        }
      for (; i <= steps[2]; ++i)
        {
        printf("-");
        }
      for (; i <= 60; ++i)
        {
        printf(" ");
        }
      printf("]\r");
      }
  private:
    const char *name;
    int steps[3];
  };

static int fetch_progress(const git_transfer_progress *stats, void *payload)
  {
  Progress &progress = *reinterpret_cast<Progress*>(payload);
  progress.update(2, stats->received_objects, stats->total_objects);
  progress.update(1, stats->indexed_objects, stats->total_objects);
  progress.print();
  return 0;
  }

static void checkout_progress(
    const char *path,
    size_t cur,
    size_t tot,
    void *payload)
  {
  Progress &progress = *reinterpret_cast<Progress*>(payload);
  progress.update(0, cur, tot);
  progress.print();
  }

static int cred_acquire(
    git_cred **cred,
    const char *url,
    const char *username_from_url,
    unsigned int allowed_types,
    void *payload)
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

char const * Git::name() const
  {
  return "git";
  }

void Git::download(const Implementation& impl, bool requested)
  {
  std::string href = impl.values()["href"];
  std::string hash = impl.values()["tag"];
  std::string path = impl.name();

  Progress progress(path.c_str());

  git_checkout_opts checkout_opts;
  memset(&checkout_opts, 0, sizeof(checkout_opts));
  checkout_opts.version = GIT_CHECKOUT_OPTS_VERSION;
  checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
  checkout_opts.progress_cb = checkout_progress;
  checkout_opts.progress_payload = &progress;

  git_repository* repo = 0;
  git_remote* origin = 0;
  git_object* object = 0;

  at_scope_exit<git_repository> repository_free(repo, git_repository_free);
  at_scope_exit<git_remote> remote_free(origin, git_remote_free);
  at_scope_exit<git_object> object_free(object, git_object_free);

  if (git_repository_open(&repo, path.c_str()) == 0)
    {
    CALL_GIT(git_remote_load, &origin, repo, "origin");
    if (std::strcmp(git_remote_url(origin), href.c_str()) != 0)
      {
      throw std::runtime_error("different origin");
      }
    }
  else
    {
    CALL_GIT(git_repository_init, &repo, path.c_str(), false);
    CALL_GIT(git_remote_create, &origin, repo, "origin", href.c_str());
    }
  git_remote_set_update_fetchhead(origin, 0);
  git_remote_set_cred_acquire_cb(origin, cred_acquire, 0);
  CALL_GIT(git_remote_connect, origin, GIT_DIRECTION_FETCH);

  at_scope_exit<git_remote> remote_disconnect(origin, git_remote_disconnect);

  CALL_GIT(git_remote_download, origin, fetch_progress, &progress);
  CALL_GIT(git_remote_update_tips, origin);
  CALL_GIT(git_revparse_single, &object, repo, hash.c_str());
  CALL_GIT(git_repository_set_head_detached, repo, git_object_id(object));
  CALL_GIT(git_checkout_tree, repo, object, &checkout_opts);
  }

} // namepsace karrot
