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

#ifdef _WIN32
#  include "windows/library.hpp"

typedef HRESULT (*GetKnownFolderPath)(GUID const&, DWORD, HANDLE, PWSTR*);
typedef HRESULT (*GetFolderPath)(HWND, int, HANDLE, DWORD, LPWSTR);

#else
#  include <curl/curl.h>
#endif

namespace Karrot
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

static boost::filesystem::path cache_dir()
  {
#ifdef _WIN32
  Library shell32("Shell32.dll");
  GetKnownFolderPath get_known_folder = nullptr;
  shell32.load("SHGetKnownFolderPath", get_known_folder);
  static const GUID local_app_data =
    {
    0xF1B32785,
    0x6FBA,
    0x4FCF,
    {0x9D, 0x55, 0x7B, 0x8E, 0x7F, 0x15, 0x70, 0x91}
    };
  if(get_known_folder)
    {
    PWSTR pwstr;
    HRESULT result = get_known_folder(local_app_data, 0, NULL, &pwstr);
    if (FAILED(result))
      {
      std::error_code error(GetLastError(), std::system_category());
      throw std::system_error(error);
      }
    boost::filesystem::path cache(pwstr);
    CoTaskMemFree(pwstr);
    return cache;
    }
  GetFolderPath get_folder = nullptr;
  shell32.require("SHGetFolderPathW", get_folder);
  wchar_t path[MAX_PATH];
  HRESULT result = get_folder(0, 0x001c, 0, 0, path);
  if (FAILED(result))
    {
    std::error_code error(GetLastError(), std::system_category());
    throw std::system_error(error);
    }
  return path;
#else
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
#endif
  }

FeedCache::FeedCache() :
    path(cache_dir() / "Karrot"),
    curl_handle(curl_easy_init())
  {
  create_directory(path);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_fun);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "karrot/0.1");
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1);
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

} // namespace Karrot
