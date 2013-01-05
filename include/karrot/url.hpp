/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_URL_HPP
#define KARROT_URL_HPP

namespace karrot
{

class Url
  {
  public:
    Url()
      {
      }
    Url(const char* str, Url* base = 0);
    int scheme;
    int user_info;
    int host;
    int port;
    int path;
    int query;
    int fragment;
  };

} // namespace karrot

#endif /* KARROT_URL_HPP */
