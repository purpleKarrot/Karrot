/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef _WIN32

#include "url.hpp"
#include <cassert>
#include <memory>
#include <curl/curl.h>

namespace
{

static size_t write_fun(char* ptr, size_t size, size_t nmemb, void* userdata)
  {
  assert(size == 1);
  std::vector<char>& buffer = *reinterpret_cast<std::vector<char>*>(userdata);
  buffer.insert(buffer.end(), ptr, ptr + nmemb);
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
    void download(std::string const& url, std::vector<char>& result) const;
  private:
    std::unique_ptr<CURL, void (*)(CURL*)> curl_handle;
  };

void Downloader::download(std::string const& url, std::vector<char>& result) const
  {
  curl_easy_setopt(curl_handle.get(), CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl_handle.get(), CURLOPT_WRITEDATA, &result);
  CURLcode res = curl_easy_perform(curl_handle.get());
  if (res != CURLE_OK)
    {
    throw std::runtime_error(curl_easy_strerror(res));
    }
  }

} // namespace

namespace Karrot
{

std::vector<char> download(std::string const& url)
  {
  std::vector<char> result;
  static Downloader downloader;
  downloader.download(url, result);
  return result;
  }

} // namespace Karrot

#endif /* _WIN32 */
