/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "subversion.hpp"

#include <boost/filesystem.hpp>

#include <cassert>
#include <cstring>
#include <iostream>

#define SVN_DEPRECATED
#include <svn_client.h>
#include <svn_cmdline.h>
#include <svn_path.h>
#include <svn_pools.h>
#include <svn_config.h>
#include <svn_fs.h>

namespace Karrot
{

class SubversionError: public std::exception
  {
  public:
    SubversionError(svn_error_t* error) :
        error(error)
      {
      }
    ~SubversionError() throw()
      {
      svn_error_clear(error);
      }
  private:
    const char* what() const throw()
      {
      static char buffer[256];
      return svn_err_best_message(error, buffer, sizeof(buffer));
      }
  private:
    svn_error_t* error;
  };

struct InfoBaton
  {
    InfoBaton(const std::string& url, svn_revnum_t rev) :
        url(url), rev(rev), url_ok(false), rev_ok(false)
      {
      }
    const std::string& url;
    svn_revnum_t rev;
    bool url_ok, rev_ok;
  };

static svn_error_t* info_receiver(void* baton, const char* path,
    const svn_info_t* info, apr_pool_t *pool)
  {
  InfoBaton* info_baton = (InfoBaton*) baton;
  info_baton->url_ok = info_baton->url == info->URL;
  info_baton->rev_ok = info_baton->rev == info->rev;
  return 0;
  }

Subversion::Subversion()
  {
  if (svn_cmdline_init("karrot", NULL) != EXIT_SUCCESS)
    return;

  pool = svn_pool_create(NULL);

  svn_error_t* error = NULL;

  /* Initialize the FS library. */
  error = svn_fs_initialize(pool);
  if (error)
    goto init_error;

  /* Make sure the ~/.subversion run-time config files exist */
  error = svn_config_ensure(0, pool);
  if (error)
    goto init_error;

  /* Initialize and allocate the client_ctx object. */
  error = svn_client_create_context(&ctx, pool);
  if (error)
    goto init_error;

  /* Load the run-time config file into a hash */
  error = svn_config_get_config(&ctx->config, 0, pool);
  if (error)
    goto init_error;

  /* Make the client_ctx capable of authenticating users */
  error = svn_cmdline_create_auth_baton(
      &ctx->auth_baton,
      FALSE,
      NULL,
      NULL,
      NULL,
      FALSE,
      FALSE,
      NULL,
      NULL,
      NULL,
      pool);
  if (error)
    goto init_error;

  return;

init_error:
  svn_pool_destroy(pool);
  }

Subversion::~Subversion()
  {
  svn_pool_destroy(pool);
  }

char const * Subversion::name() const
  {
  return "subversion";
  }

void Subversion::download(const Implementation& impl, bool requested)
  {
  svn_revnum_t result_rev;
  svn_opt_revision_t revision;
  svn_opt_revision_t peg_revision;
  peg_revision.kind = svn_opt_revision_unspecified;

  std::string url = impl.values()["href"];
  std::string tag = impl.values()["tag"];

  std::string::size_type loc = tag.rfind('@');
  if (loc != std::string::npos)
    {
    revision.kind = svn_opt_revision_number;
    revision.value.number = std::atol(&tag[loc + 1]);
    tag.resize(loc);
    }
  else
    {
    revision.kind = svn_opt_revision_head;
    revision.value.number = 0;
    }
  if (tag.find("://") != std::string::npos)
    {
    url = tag;
    }
  else
    {
    url += "/tags/" + tag;
    }

  const std::string& path = impl.name();
  svn_error_t* error = NULL;

  if (!boost::filesystem::exists(path)) // / ".svn"))
    {
    error = svn_client_checkout3(
        &result_rev,
        url.c_str(),
        path.c_str(),
        &peg_revision,
        &revision,
        svn_depth_infinity,
        TRUE,  // ignore_externals
        FALSE, // allow_unver_obstructions
        ctx,
        pool);
    if (error)
      {
      throw SubversionError(error);
      }
    return;
    }
  InfoBaton info_baton(url, revision.value.number);
  error = svn_client_info2(
      path.c_str(),
      NULL,  // peg_revision
      NULL,  // revision
      info_receiver,
      &info_baton,
      svn_depth_empty,
      NULL,  // changelists
      ctx,
      pool);
  if (error)
    {
    throw SubversionError(error);
    }
  if (!info_baton.url_ok)
    {
    error = svn_client_switch2(
        &result_rev,
        path.c_str(),
        url.c_str(),
        &peg_revision,
        &revision,
        svn_depth_infinity,
        TRUE,  // depth_is_sticky,
        TRUE,  // ignore_externals
        FALSE, // allow_unver_obstructions
        ctx,
        pool);
    if (error)
      {
      throw SubversionError(error);
      }
    }
  else if(revision.kind == svn_opt_revision_number && !info_baton.rev_ok)
    {
    apr_array_header_t *paths = apr_array_make(pool, 1, sizeof(const char*));
    apr_array_header_t *result_revs;
    APR_ARRAY_PUSH(paths, const char*) = path.c_str();
    error = svn_client_update3(
        &result_revs,
        paths,
        &revision,
        svn_depth_infinity,
        TRUE,  // depth_is_sticky,
        TRUE,  // ignore_externals
        FALSE, // allow_unver_obstructions
        ctx,
        pool);
    if (error)
      {
      throw SubversionError(error);
      }
    }
  }

} // namespace Karrot
