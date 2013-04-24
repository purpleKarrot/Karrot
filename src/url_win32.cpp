/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifdef _WIN32

#include "url.hpp"
#include "quark.hpp"
#include <windows.h>
#include <wininet.h>
#include <shlwapi.h>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <system_error>
#include <vector>
#include <boost/throw_exception.hpp>

namespace
{

class Library
  {
  public:
    Library(char const* name) :
        handle(LoadLibrary(name))
      {
      if (!handle)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error, name));
        }
      }
    ~Library()
      {
      FreeLibrary(handle);
      }
  public:
    Library(Library&& other) :
        handle(other.handle)
      {
      other.handle = nullptr;
      }
    Library& operator=(Library&& other)
      {
      handle = other.handle;
      other.handle = nullptr;
      return *this;
      }
  public:
    Library(Library const&) = delete;
    Library& operator=(Library const&) = delete;
  public:
    template<typename Function>
    void load(char const* name, Function& function)
      {
      function = (Function) GetProcAddress(handle, name);
      }
    template<typename Function>
    void require(char const* name, Function& function)
      {
      load(name, std::forward<Function&>(function));
      if (!function)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error, name));
        }
      }
  private:
    HMODULE handle;
  };

class WinHTTP: private Library
  {
  public:
    struct AutoProxyOptions
      {
      DWORD flags;
      DWORD auto_detect_flags;
      LPCWSTR auto_config_url;
      LPVOID reserved1;
      DWORD reserved2;
      BOOL auto_logon_if_challenged;
      };
    struct ProxyInfo
      {
      DWORD access_type;
      LPWSTR proxy;
      LPWSTR proxy_bypass;
      };
  public:
    WinHTTP() : Library("winhttp.dll")
      {
      require("WinHttpCloseHandle", close_handle);
      require("WinHttpConnect", connect);
      require("WinHttpOpen", open);
      require("WinHttpOpenRequest", open_request);
      require("WinHttpQueryDataAvailable", query_data_available);
      require("WinHttpReadData", read_data);
      require("WinHttpReceiveResponse", receive_response);
      require("WinHttpSendRequest", send_request);
      require("WinHttpGetProxyForUrl", get_proxy_for_url);
      require("WinHttpSetOption", set_option);
      }
  public:
    BOOL (WINAPI *close_handle) (HINTERNET);
    HINTERNET (WINAPI *connect) (HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
    HINTERNET (WINAPI *open) (LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
    HINTERNET (WINAPI *open_request) (HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR*, DWORD);
    BOOL (WINAPI *query_data_available) (HINTERNET, LPDWORD);
    BOOL (WINAPI *read_data) (HINTERNET, LPVOID, DWORD, LPDWORD);
    BOOL (WINAPI *receive_response) (HINTERNET, LPVOID);
    BOOL (WINAPI *send_request) (HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR);
    BOOL (WINAPI *get_proxy_for_url) (HINTERNET, LPCWSTR, AutoProxyOptions*, ProxyInfo*);
    BOOL (WINAPI *set_option) (HINTERNET, DWORD, LPVOID, DWORD);
  };

class Wide
  {
  public:
    Wide(char const *str)
      {
      int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
      if (len <= 0)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      impl.resize(len);
      MultiByteToWideChar(CP_ACP, 0, str, -1, &impl[0], impl.length());
      }
    operator LPCWSTR() const
      {
      return impl.c_str();
      }
  private:
    std::wstring impl;
  };

char const* get_host(Karrot::Url const& url)
  {
  return Karrot::quark_to_string(url.host);
  }

short get_port(Karrot::Url const& url)
  {
  if (url.port)
    {
    return std::atoi(Karrot::quark_to_string(url.port));
    }
  if (url.scheme == Karrot::string_to_quark("https"))
    {
    return 443;
    }
  return 80;
  }

class Downloader
  {
  public:
    Downloader() :
        session(win_http.open(L"Karrot/1.0", 0, 0, 0, 0), win_http.close_handle)
      {
      if (!session)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      }
  public:
    void download(Karrot::Url const& url, std::string const& filepath)
      {
      HINTERNET conn = get_connection(get_host(url), get_port(url));
      Handle request(
          win_http.open_request(
              conn,
              L"GET",
              Wide(Karrot::quark_to_string(url.path)),
              L"HTTP/1.1",
              0,
              0,
              0 /*0x00800000*/),
          win_http.close_handle);
      if (!request)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }

      WinHTTP::AutoProxyOptions auto_proxy_options;
      WinHTTP::ProxyInfo proxy_info;
      ZeroMemory(&auto_proxy_options, sizeof(auto_proxy_options));
      ZeroMemory(&proxy_info, sizeof(proxy_info));
      auto_proxy_options.flags = 0x00000001;
      auto_proxy_options.auto_detect_flags = 0x00000001 | 0x00000002;
      auto_proxy_options.auto_logon_if_challenged = TRUE;
      if (win_http.get_proxy_for_url(
          session.get(),
          Wide(Karrot::url_to_string(url).c_str()),
          &auto_proxy_options,
          &proxy_info))
        {
        if (!win_http.set_option(request.get(), 38, &proxy_info, sizeof(proxy_info)))
          {
          std::error_code error(GetLastError(), std::system_category());
          BOOST_THROW_EXCEPTION(std::system_error(error));
          }
        }

      if (!win_http.send_request(request.get(), 0, 0, 0, 0, 0, 0))
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      if (!win_http.receive_response(request.get(), 0))
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      DWORD size = 0;
      DWORD down = 0;
      std::vector<char> buffer;
      std::ofstream file(filepath);
      do
        {
        size = 0;
        if (!win_http.query_data_available(request.get(), &size))
          {
          std::error_code error(GetLastError(), std::system_category());
          BOOST_THROW_EXCEPTION(std::system_error(error));
          }
        buffer.resize(size);
        if (!win_http.read_data(request.get(), &buffer[0], size, &down))
          {
          std::error_code error(GetLastError(), std::system_category());
          BOOST_THROW_EXCEPTION(std::system_error(error));
          }
        file.write(buffer.data(), down);
        }
      while (down > 0);
      }
  private:
    HINTERNET get_connection(std::string const& host, short port)
      {
      for (const Connection& conn : connections)
        {
        if (conn.host == host && conn.port == port)
          {
          return conn.handle.get();
          }
        }
      Handle handle(
          win_http.connect(session.get(), Wide(host.c_str()), port, 0),
          win_http.close_handle);
      if (!handle)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      connections.emplace_back(host, port, std::move(handle));
      return connections.back().handle.get();
      }
  private:
    typedef std::unique_ptr<void, BOOL (WINAPI*) (HINTERNET)> Handle;

    template<typename Function, typename... Args>
    Handle make_handle(Function function, Args... args)
      {
      HINTERNET handle = function(args...);
      if (!handle)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      return Handle(handle, win_http.close_handle);
      }
  private:
    WinHTTP win_http;
    Handle session;
    struct Connection
      {
      Connection(std::string const& host, short port, Handle&& handle) :
          host(host), port(port), handle(std::move(handle))
        {
        }
      std::string host;
      short port;
      Handle handle;
      };
    std::vector<Connection> connections;
  };

bool file_exists(char const *file_name)
  {
  DWORD attr = GetFileAttributesA(file_name);
  if (attr == INVALID_FILE_ATTRIBUTES)
    {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND)
      {
      return false;
      }
    std::error_code error_code(error, std::system_category());
    BOOST_THROW_EXCEPTION(std::system_error(error_code));
    }
  return true;
  }

} // namespace

namespace Karrot
{

std::string download(Url const& url, std::string const& feed_cache, bool force)
  {
  if (url.scheme == string_to_quark("file"))
    {
    return quark_to_string(url.path);
    }
  char filepath[MAX_PATH];
  if (GetFullPathNameA(feed_cache.c_str(), MAX_PATH, filepath, nullptr) == 0)
    {
    std::error_code error(GetLastError(), std::system_category());
    BOOST_THROW_EXCEPTION(std::system_error(error));
    }
  if (FAILED(PathAppendA(filepath, url_to_filename(url).c_str())))
    {
    std::error_code error(GetLastError(), std::system_category());
    BOOST_THROW_EXCEPTION(std::system_error(error));
    }
  if (force || !file_exists(filepath))
    {
    static Downloader downloader;
    downloader.download(url, filepath);
    }
  return filepath;
  }

} // namespace Karrot

#endif /* _WIN32 */
