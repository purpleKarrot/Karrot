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
#include <shlobj.h>
#include <shlwapi.h>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <system_error>
#include <vector>

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
        throw std::system_error(error, name);
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
        throw std::system_error(error, name);
        }
      }
  private:
    HMODULE handle;
  };

class WinHTTP: private Library
  {
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
      }
  public:
    BOOL (WINAPI *close_handle) (HINTERNET);
    HINTERNET (WINAPI *connect) (HINTERNET, LPCSTR, INTERNET_PORT, DWORD);
    HINTERNET (WINAPI *open) (LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD);
    HINTERNET (WINAPI *open_request) (HINTERNET, LPCSTR, LPCSTR, LPCSTR, LPCSTR, LPCSTR*, DWORD);
    BOOL (WINAPI *query_data_available) (HINTERNET, LPDWORD);
    BOOL (WINAPI *read_data) (HINTERNET, LPVOID, DWORD, LPDWORD);
    BOOL (WINAPI *receive_response) (HINTERNET, LPVOID);
    BOOL (WINAPI *send_request) (HINTERNET, LPCSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR);
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
        session(win_http.open("Karrot/1.0", 0, 0, 0, 0), win_http.close_handle)
      {
      }
  public:
    void download(Karrot::Url const& url, std::ostream& file)
      {
      HINTERNET conn = get_connection(get_host(url), get_port(url));
      Handle request(
          win_http.open_request(
              conn,
              "GET",
              Karrot::quark_to_string(url.path),
              0,
              0,
              0,
              0x00800000),
          win_http.close_handle);
      if (!win_http.send_request(request.get(), 0, 0, 0, 0, 0, 0))
        {
        std::error_code error(GetLastError(), std::system_category());
        throw std::system_error(error);
        }
      if (!win_http.receive_response(request.get(), 0))
        {
        std::error_code error(GetLastError(), std::system_category());
        throw std::system_error(error);
        }
      DWORD size = 0;
      DWORD down = 0;
      std::vector<char> buffer;
      do
        {
        size = 0;
        if (!win_http.query_data_available(request.get(), &size))
          {
          std::error_code error(GetLastError(), std::system_category());
          throw std::system_error(error);
          }
        buffer.resize(size);
        if (!win_http.read_data(request.get(), &buffer[0], size, &down))
          {
          std::error_code error(GetLastError(), std::system_category());
          throw std::system_error(error);
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
          win_http.connect(session.get(), host.c_str(), port, 0),
          win_http.close_handle);
      connections.emplace_back(host, port, std::move(handle));
      return handle.get();
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
        throw std::system_error(error);
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


class Handle
  {
  public:
    Handle(HANDLE handle) : handle(handle)
      {
      }
    ~Handle()
      {
      if (handle != INVALID_HANDLE_VALUE)
        {
        CloseHandle(handle);
        }
      }
    operator HANDLE() const
      {
      return handle;
      }
  private:
    HANDLE handle;
  };

bool is_cached(char const *file_name)
  {
  DWORD attr = GetFileAttributesA(file_name);
  if (attr == INVALID_FILE_ATTRIBUTES)
    {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND)
      {
      return false;
      }
    throw std::system_error(std::error_code(error, std::system_category()));
    }
  Handle handle = CreateFileA(
      file_name,
      0,
      FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
      0,
      OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS,
      0);
  if (handle == INVALID_HANDLE_VALUE)
    {
    std::error_code error(GetLastError(), std::system_category());
    throw std::system_error(error);
    }
  FILETIME write_time;
  if (GetFileTime(handle, 0, 0, &write_time) == 0)
    {
    std::error_code error(GetLastError(), std::system_category());
    throw std::system_error(error);
    }
  FILETIME current_time;
  GetSystemTimeAsFileTime(&current_time);
  ULARGE_INTEGER write_large, current_large;
  write_large.LowPart = write_time.dwLowDateTime;
  write_large.HighPart = write_time.dwHighDateTime;
  current_large.LowPart = current_time.dwLowDateTime;
  current_large.HighPart = current_time.dwHighDateTime;
  return current_large.QuadPart - write_large.QuadPart < 864000000000LL;
  }

} // namespace

namespace Karrot
{

std::string download(Url const& url)
  {
  if (url.scheme == string_to_quark("file"))
    {
    return quark_to_string(url.path);
    }
  char filepath[MAX_PATH];
  HRESULT result = SHGetFolderPathA(0, CSIDL_INTERNET_CACHE, 0, 0, filepath);
  if (FAILED(result))
    {
    std::error_code error(GetLastError(), std::system_category());
    throw std::system_error(error);
    }
  std::string str = quark_to_string(url.host);
  str += quark_to_string(url.path);
  for (char& c : str)
    {
    if (c == '/')
      {
      c = '-';
      }
    }
  result = PathAppendA(filepath, str.c_str());
  if (FAILED(result))
    {
    std::error_code error(GetLastError(), std::system_category());
    throw std::system_error(error);
    }
  if (!is_cached(filepath))
    {
    static Downloader downloader;
    std::ofstream file(filepath);
    downloader.download(url, file);
    }
  return filepath;
  }

} // namespace Karrot

#endif /* _WIN32 */
