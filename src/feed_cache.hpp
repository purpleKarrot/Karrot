/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_FEED_CACHE_HPP
#define KARROT_FEED_CACHE_HPP

#include <boost/filesystem/path.hpp>

namespace karrot
{

class Url;

class FeedCache : boost::noncopyable
  {
  public:
    FeedCache();
    ~FeedCache();
    boost::filesystem::path local_path(const Url& url);
  private:
    boost::filesystem::path path;
    void* curl_handle;
  };

} // namespace karrot

#endif /* KARROT_FEED_CACHE_HPP */
