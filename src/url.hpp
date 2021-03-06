/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_URL_HPP
#define KARROT_URL_HPP

#include <string>

namespace Karrot
{

std::string url_to_filename(std::string url);
std::string resolve_uri(std::string const& base, std::string const& relative);
std::string download(std::string const& url, std::string const& feed_cache, bool force);

} // namespace Karrot

#endif /* KARROT_URL_HPP */
