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
#include <vector>

namespace Karrot
{

std::string resolve_uri(std::string const& base, std::string const& relative);
std::vector<char> download(std::string const& url);

} // namespace Karrot

#endif /* KARROT_URL_HPP */
