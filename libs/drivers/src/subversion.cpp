/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/subversion.hpp>
#include <karrot/dictionary.hpp>

#define SVN_DEPRECATED
#include <svn_client.h>
#include <svn_cmdline.h>
#include <svn_path.h>
#include <svn_pools.h>
#include <svn_config.h>
#include <svn_fs.h>
#include <cstring>

namespace Karrot
{

struct InfoBaton
{
	char const *url;
	svn_revnum_t rev;
	int url_ok, rev_ok;
};

static svn_error_t *
info_receiver(void *baton, const char *path, const svn_info_t *info, apr_pool_t *pool)
{
	struct InfoBaton* info_baton = (struct InfoBaton*) baton;
	info_baton->url_ok = strcmp(info_baton->url, info->URL) == 0;
	info_baton->rev_ok = info_baton->rev == info->rev;
	return 0;
}

Subversion::Subversion() :
		Driver("subversion", "http://purplekarrot.net/2013/subversion")
{
	svn_error_t *error = nullptr;

	if (svn_cmdline_init("karrot", nullptr) != EXIT_SUCCESS)
	{
		throw std::runtime_error("failed to initialize svn");
	}

	pool = svn_pool_create(nullptr);

	/* Initialize the FS library. */
	error = svn_fs_initialize(pool);
	if (error)
	{
		goto error;
	}

	/* Make sure the ~/.subversion run-time config files exist */
	error = svn_config_ensure(0, pool);
	if (error)
	{
		goto error;
	}

	/* Initialize and allocate the client_ctx object. */
	error = svn_client_create_context(&client, pool);
	if (error)
	{
		goto error;
	}

	/* Load the run-time config file into a hash */
	error = svn_config_get_config(&client->config, 0, pool);
	if (error)
	{
		goto error;
	}

	/* Make the client_ctx capable of authenticating users */
	error = svn_cmdline_create_auth_baton(
		&client->auth_baton,
		false,
		nullptr,
		nullptr,
		nullptr,
		false,
		false,
		nullptr,
		nullptr,
		nullptr,
		pool);
	if (error)
	{
		goto error;
	}

	return;

error:
	svn_pool_destroy(pool);
}

Subversion::~Subversion()
{
	svn_pool_destroy(pool);
}

void Subversion::do_handle(Module const& impl)
{
	svn_revnum_t result_rev;
	svn_opt_revision_t revision;
	svn_opt_revision_t peg_revision;
	peg_revision.kind = svn_opt_revision_unspecified;
	svn_error_t* error = nullptr;

	char const *url = Karrot::get(impl.values, "href");
	char const *tag = Karrot::get(impl.values, "tag");
	char const *path = Karrot::get(*impl.meta, "name");

	char const *revchr = std::strrchr(tag, '@');
	if (revchr != nullptr)
	{
		revision.kind = svn_opt_revision_number;
		revision.value.number = apr_atoi64(revchr + 1);
		tag = apr_pstrndup(this->pool, tag, revchr - tag);
	}
	else
	{
		revision.kind = svn_opt_revision_head;
		revision.value.number = 0;
	}

	if (strstr(tag, "://") == nullptr)
	{
		url = tag;
	}
	else
	{
		url = apr_pstrcat(this->pool, url, "/tags/", tag, NULL);
	}

	apr_finfo_t finfo;
	apr_status_t statcode;
	statcode = apr_stat(&finfo, path /*TODO: ".svn"*/, APR_FINFO_TYPE, this->pool);
	if (statcode != APR_SUCCESS)
	{
		char buffer[256];
		puts(apr_strerror(statcode, buffer, sizeof(buffer)));
		return;
	}

	if (finfo.filetype == APR_NOFILE)
	{
		error = svn_client_checkout(
			&result_rev,
			url,
			path,
			&revision,
			true, // recurse
			this->client,
			this->pool);
		if (error)
		{
			throw std::runtime_error(error->message);
		}

		return;
	}

	struct InfoBaton info_baton = {url, revision.value.number, false, false};
	error = svn_client_info(
		path,
		nullptr,  // peg_revision
		nullptr,  // revision
		info_receiver,
		&info_baton,
		true, // recurse
		this->client,
		this->pool);
	if (error)
	{
		throw std::runtime_error(error->message);
	}

	if (!info_baton.url_ok)
	{
		error = svn_client_switch(
			&result_rev,
			path,
			url,
			&revision,
			true, // recurse
			this->client,
			this->pool);
		if (error)
		{
			throw std::runtime_error(error->message);
		}
	}
	else if(revision.kind == svn_opt_revision_number && !info_baton.rev_ok)
	{
		error = svn_client_update(
			&result_rev,
			path,
			&revision,
			true, // recurse
			this->client,
			this->pool);
		if (error)
		{
			throw std::runtime_error(error->message);
		}
	}
}

} // namespace Karrot