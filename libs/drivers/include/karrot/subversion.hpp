/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_SUBVERSION_HPP
#define KARROT_SUBVERSION_HPP

#include <karrot/driver.hpp>

struct apr_pool_t;
struct svn_client_ctx_t;

namespace Karrot
{

class Subversion: public Driver
{
public:
	Subversion();
	~Subversion();

private:
	void do_handle(Module const& module) override;

private:
	apr_pool_t *pool;
	svn_client_ctx_t *client;
};

} // namespace Karrot

#endif /* KARROT_SUBVERSION_HPP */