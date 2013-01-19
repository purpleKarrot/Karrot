/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "feed_cache.hpp"
#include "url.hpp"
#include "quark.hpp"
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <curl/curl.h>

namespace karrot
{

static std::string url_to_string(const Url& url)
  {
  std::string str = quark_to_string(url.scheme);
  str += "://";
  if (url.user_info)
    {
    str += quark_to_string(url.user_info);
    str += "@";
    }
  str += quark_to_string(url.host);
  if (url.port)
    {
    str += ":";
    str += quark_to_string(url.port);
    }
  str += quark_to_string(url.path);
  return str;
  }

static std::string url_to_filename(const Url& url)
  {
  std::string str = quark_to_string(url.host);
  str += quark_to_string(url.path);
  for (char& c : str)
    {
    if (c == '/')
      {
      c = '-';
      }
    }
  return str;
  }

size_t write_fun(char* ptr, size_t size, size_t nmemb, void* userdata)
  {
  assert(size == 1);
  using boost::filesystem::ofstream;
  ofstream& file = *reinterpret_cast<ofstream*>(userdata);
  file.write(ptr, static_cast<std::streamsize>(nmemb));
  return nmemb;
  }

static int prog_fun(void *clientp, double total, double now, double t, double n)
  {
  int x;
  int w = 60;
  double ratio = (now == 0) ? 0 : now / total;
  int c = (int) (ratio * w);
  printf("                                                                 \r");
  printf("%3d%% [", (int) (ratio * 100));
  for (x = 0; x < c; x++)
    {
    printf("=");
    }
  for (x = c; x < w; x++)
    {
    printf(" ");
    }
  printf("]\r");
  fflush (stdout);
  return 0;
  }

FeedCache::FeedCache() :
    path(boost::filesystem::current_path() / "cache"),
    curl_handle(curl_easy_init())
  {
  create_directory(path);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_fun);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "karrot/0.1");
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl_handle, CURLOPT_PROGRESSFUNCTION, prog_fun);
  curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0);
  }

FeedCache::~FeedCache()
  {
  curl_easy_cleanup(curl_handle);
  }

boost::filesystem::path FeedCache::local_path(const Url& url)
  {
  boost::filesystem::path filepath = path / url_to_filename(url);
  if (!exists(filepath) || last_write_time(filepath) < std::time(0) - 86400)
    {
    boost::filesystem::ofstream file(filepath, std::ios::binary);
    curl_easy_setopt(curl_handle, CURLOPT_URL, url_to_string(url).c_str());
    curl_easy_setopt(curl_handle, CURLOPT_FILE, &file);
    CURLcode res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK)
      {
      file.close();
      remove(filepath);
      throw std::runtime_error(curl_easy_strerror(res));
      }
    file << std::flush;
    }
  return filepath;
  }

} // namespace karrot
