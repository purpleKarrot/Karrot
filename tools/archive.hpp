/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_ARCHIVE_HPP
#define KARROT_ARCHIVE_HPP

#include <karrot.h>
#include <curl/curl.h>
#include <proxy.h>

namespace Karrot
{

class Archive: public Driver
{
public:
	Archive();
	~Archive();

private:
	void do_filter(KImplementation& impl, Add add) const override;
	void do_handle(KImplementation const& impl, bool requested) const override;
	void download(const char* path, const char* url, const char* checksum) const;

private:
	CURL* curl;
	pxProxyFactory* proxy_factory;
};

} // namespace Karrot

#endif /* KARROT_ARCHIVE_HPP */
