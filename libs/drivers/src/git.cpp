/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <karrot/git.hpp>
#include <git2.h>
#include <cstring>
#include <cstdio>
#include <karrot/dictionary.hpp>
#include <karrot/implementation.hpp>

namespace Karrot
{

static int cred_acquire(git_cred **out, const char *url,
		const char *username_from_url, unsigned int allowed_types,
		void *payload)
{
	char username[128] = { 0 };
	char password[128] = { 0 };

	printf("Username: ");
	scanf("%s", username);

	/* Yup. Right there on your terminal. Careful where you copy/paste output. */
	printf("Password: ");
	scanf("%s", password);

	return git_cred_userpass_plaintext_new(out, username, password);
}

static int transfer_progress(const git_transfer_progress *stats, void *payload)
{
//	progress_data *pd = (progress_data*)payload;
//	pd->fetch_progress = *stats;
//	print_progress(pd);
	return 0;
}

static void checkout_progress(const char *path, size_t cur, size_t tot, void *payload)
{
//	progress_data *pd = (progress_data*)payload;
//	pd->completed_steps = cur;
//	pd->total_steps = tot;
//	pd->path = path;
//	print_progress(pd);
}

Git::Git() :
		Driver("git", "http://purplekarrot.net/2013/git")
{
}

void Git::do_handle(Implementation const& impl, bool requested) const
{
	char const *url = Karrot::get(impl.values, "href");
	char const *tag = Karrot::get(impl.values, "tag");
	char const *path = Karrot::get(*impl.meta, "name");

	git_repository* repo = NULL;
	git_remote* origin = NULL;

	if (git_repository_open(&repo, path) == GIT_OK)
	{
		if (git_remote_load(&origin, repo, "origin") != GIT_OK)
		{
			git_repository_free(repo);
			throw std::runtime_error(giterr_last()->message);
		}

		if (std::strcmp(url, git_remote_url(origin)) != 0)
		{
			giterr_set_str(GITERR_INVALID, "different origin");
			git_repository_free(repo);
			throw std::runtime_error(giterr_last()->message);
		}
	}
	else
	{
		if (git_repository_init(&repo, path, 0) != GIT_OK)
		{
			throw std::runtime_error(giterr_last()->message);
		}

		if (git_remote_create(&origin, repo, "origin", url) != GIT_OK)
		{
			git_repository_free(repo);
			throw std::runtime_error(giterr_last()->message);
		}
	}

	git_remote_set_update_fetchhead(origin, 0);
	git_remote_set_cred_acquire_cb(origin, cred_acquire, NULL);

	if (git_remote_connect(origin, GIT_DIRECTION_FETCH) != GIT_OK)
	{
		git_remote_free(origin);
		git_repository_free(repo);
		throw std::runtime_error(giterr_last()->message);
	}

	if (git_remote_download(origin, transfer_progress, NULL) != GIT_OK)
	{
		git_remote_free(origin);
		git_repository_free(repo);
		throw std::runtime_error(giterr_last()->message);
	}

	if (git_remote_update_tips(origin) != GIT_OK)
	{
		git_remote_free(origin);
		git_repository_free(repo);
		throw std::runtime_error(giterr_last()->message);
	}

	git_object* object = NULL;

	if (git_revparse_single(&object, repo, tag) != GIT_OK)
	{
		git_remote_free(origin);
		git_repository_free(repo);
		throw std::runtime_error(giterr_last()->message);
	}

	if (git_repository_set_head_detached(repo, git_object_id(object)) != GIT_OK)
	{
		git_object_free(object);
		git_remote_free(origin);
		git_repository_free(repo);
		throw std::runtime_error(giterr_last()->message);
	}

	git_checkout_opts checkout_opts = GIT_CHECKOUT_OPTS_INIT;
	checkout_opts.version = GIT_CHECKOUT_OPTS_VERSION;
	checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
	checkout_opts.progress_cb = checkout_progress;
	checkout_opts.progress_payload = NULL;

	if (git_checkout_tree(repo, object, &checkout_opts) != GIT_OK)
	{
		git_object_free(object);
		git_remote_free(origin);
		git_repository_free(repo);
		throw std::runtime_error(giterr_last()->message);
	}

	git_object_free(object);
	git_remote_free(origin);
	git_repository_free(repo);
}

} // namespace Karrot
