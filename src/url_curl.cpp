/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef _WIN32

#include "url.hpp"
#include <memory>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <curl/curl.h>

namespace
{

size_t write_fun(char* ptr, size_t size, size_t nmemb, void* userdata)
  {
  assert(size == 1);
  std::ofstream& file = *reinterpret_cast<std::ofstream*>(userdata);
  file.write(ptr, static_cast<std::streamsize>(nmemb));
  return nmemb;
  }

class Downloader
  {
  public:
    Downloader() :
        curl_handle(curl_easy_init(), curl_easy_cleanup)
      {
      curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEFUNCTION, write_fun);
      curl_easy_setopt(curl_handle.get(), CURLOPT_USERAGENT, "Karrot/0.1");
      curl_easy_setopt(curl_handle.get(), CURLOPT_FOLLOWLOCATION, 1);
      }
    void download(std::string const& url, std::ostream& file) const;
  private:
    std::unique_ptr<CURL, void (*)(CURL*)> curl_handle;
  };

void Downloader::download(std::string const& url, std::ostream& file) const
  {
  curl_easy_setopt(curl_handle.get(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_handle.get(), CURLOPT_FILE, &file);
  CURLcode res = curl_easy_perform(curl_handle.get());
  if (res != CURLE_OK)
    {
    throw std::runtime_error(curl_easy_strerror(res));
    }
  }

} // namespace

namespace Karrot
{

std::string download(std::string const& url, std::string const& feed_cache, bool force)
  {
  namespace fs = boost::filesystem;
  fs::path filepath = fs::path(feed_cache) / url_encode(url);
  if (force || !exists(filepath))
    {
    static Downloader downloader;
    try
      {
      fs::ofstream file(filepath, std::ios::binary);
      downloader.download(url, file);
      }
    catch (...)
      {
      remove(filepath);
      throw;
      }
    }
  return filepath.string();
  }

} // namespace Karrot

#endif /* _WIN32 */
