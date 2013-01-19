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
      static char buffer[1024];
      return svn_err_best_message(svn_error_purge_tracing(error), buffer, 1024);
      }
  private:
    svn_error_t* error;
  };

struct InfoBaton
  {
    InfoBaton(const char* url, svn_revnum_t rev) :
        url(url), rev(rev), url_ok(false), rev_ok(false)
      {
      }
    const char* url;
    svn_revnum_t rev;
    bool url_ok, rev_ok;
  };

static svn_error_t* info_receiver(void* baton, const char* path,
    const svn_client_info2_t* info, apr_pool_t *pool)
  {
  InfoBaton* info_baton = (InfoBaton*) baton;
  info_baton->url_ok = std::strcmp(info_baton->url, info->URL) == 0;
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

void Subversion::download(const Implementation& impl, bool requested)
  {
  svn_revnum_t result_rev;
  svn_opt_revision_t revision;
  svn_opt_revision_t peg_revision;
  peg_revision.kind = svn_opt_revision_unspecified;

  std::string url = impl.values.at("href");
  std::string tag = impl.values.at("tag");

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

  const char* abs_path;
  svn_dirent_get_absolute(&abs_path, impl.name.c_str(), pool);
  const char* canonical_url = svn_uri_canonicalize(url.c_str(), pool);
  const char* canonical_path = svn_dirent_canonicalize(abs_path, pool);

  svn_error_t* error = NULL;

  if (!boost::filesystem::exists(abs_path)) // / ".svn"))
    {
    error = svn_client_checkout3(
        &result_rev,
        canonical_url,
        canonical_path,
        &peg_revision,
        &revision,
        svn_depth_infinity,
        TRUE, //ignore_externals
        FALSE, //allow_unver_obstructions
        ctx,
        pool);
    if (error)
      {
      throw SubversionError(error);
      }
    return;
    }
  InfoBaton info_baton(canonical_url, revision.value.number);
  error = svn_client_info3(
      abs_path,
      NULL,
      NULL,
      svn_depth_empty,
      FALSE,
      FALSE,
      NULL,
      info_receiver,
      &info_baton,
      ctx,
      pool);
  if (error)
    {
    throw SubversionError(error);
    }
  if (!info_baton.url_ok)
    {
    error = svn_client_switch3(
        &result_rev,
        canonical_path,
        canonical_url,
        &peg_revision,
        &revision,
        svn_depth_infinity,
        TRUE,  // depth_is_sticky,
        TRUE,  // ignore_externals
        FALSE, // allow_unver_obstructions
        FALSE, // ignore_ancestry
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
    APR_ARRAY_PUSH(paths, const char*) = canonical_path;
    error = svn_client_update4(
        &result_revs,
        paths,
        &revision,
        svn_depth_infinity,
        TRUE,  // depth_is_sticky,
        TRUE,  // ignore_externals
        FALSE, // allow_unver_obstructions
        TRUE,  // adds_as_modification,
        FALSE, // make_parents
        ctx,
        pool);
    if (error)
      {
      throw SubversionError(error);
      }
    }
  }

} // namespace Karrot
