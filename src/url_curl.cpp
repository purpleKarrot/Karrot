/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef _WIN32

#include "url.hpp"
#include "quark.hpp"
#include <memory>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <curl/curl.h>

namespace
{

std::string url_to_string(const Karrot::Url& url)
  {
  std::string str = Karrot::quark_to_string(url.scheme);
  str += "://";
  if (url.user_info)
    {
    str += Karrot::quark_to_string(url.user_info);
    str += "@";
    }
  str += Karrot::quark_to_string(url.host);
  if (url.port)
    {
    str += ":";
    str += Karrot::quark_to_string(url.port);
    }
  str += Karrot::quark_to_string(url.path);
  return str;
  }

std::string url_to_filename(const Karrot::Url& url)
  {
  std::string str = Karrot::quark_to_string(url.host);
  str += Karrot::quark_to_string(url.path);
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

boost::filesystem::path cache_dir()
  {
  const char* cache = getenv("XDG_CACHE_HOME");
  if (cache)
    {
    return cache;
    }
  const char* home = getenv("HOME");
  if (home)
    {
    return boost::filesystem::path(home) / ".config";
    }
  return boost::filesystem::current_path();
  }

boost::filesystem::path make_cache_dir()
  {
  boost::filesystem::path cache = cache_dir() / "Karrot";
  create_directory(cache);
  return std::move(cache);
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
    void download(Karrot::Url const& url, std::ostream& file) const;
  private:
    std::unique_ptr<CURL, void (*)(CURL*)> curl_handle;
  };

void Downloader::download(const Karrot::Url& url, std::ostream& file) const
  {
  curl_easy_setopt(curl_handle.get(), CURLOPT_URL, url_to_string(url).c_str());
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

std::string download(Url const& url)
  {
  static boost::filesystem::path cache = make_cache_dir();
  boost::filesystem::path filepath = cache / url_to_filename(url);
  if (!exists(filepath) || last_write_time(filepath) < std::time(0) - 86400)
    {
    static Downloader downloader;
    try
      {
      boost::filesystem::ofstream file(filepath, std::ios::binary);
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
