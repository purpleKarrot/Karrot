/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifdef _WIN32

#include "url.hpp"
#include <windows.h>
#include <winhttp.h>
#include <fstream>
#include <memory>
#include <system_error>
#include <boost/throw_exception.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace
{

class Downloader
  {
  public:
    Downloader() :
        session(WinHttpOpen(L"Karrot/1.0", 0, 0, 0, 0), WinHttpCloseHandle)
      {
      if (!session)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      }
  public:
    void download(std::string const& url, std::vector<char>& result)
      {
      int wide_url_length = MultiByteToWideChar(CP_ACP, 0, url.c_str(), url.length(), NULL, 0);
      if (wide_url_length <= 0)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      std::wstring wide_url(wide_url_length, L'\0');
      MultiByteToWideChar(CP_ACP, 0, url.c_str(), url.length(), &wide_url[0], wide_url_length);
      URL_COMPONENTS url_components = {sizeof(URL_COMPONENTS)};
      url_components.dwSchemeLength = -1;
      url_components.dwHostNameLength = -1;
      url_components.dwUrlPathLength = -1;
      if (!WinHttpCrackUrl(wide_url.c_str(), wide_url_length, 0, &url_components))
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      HINTERNET conn = get_connection(
          std::wstring(url_components.lpszHostName, url_components.dwHostNameLength),
          url_components.nPort);
      Handle request(
          WinHttpOpenRequest(conn, L"GET", url_components.lpszUrlPath, L"HTTP/1.1", 0, 0, 0),
          WinHttpCloseHandle);
      if (!request)
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }

      WINHTTP_AUTOPROXY_OPTIONS auto_proxy_options;
      WINHTTP_PROXY_INFO proxy_info;
      ZeroMemory(&auto_proxy_options, sizeof(auto_proxy_options));
      ZeroMemory(&proxy_info, sizeof(proxy_info));
      auto_proxy_options.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
      auto_proxy_options.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
      auto_proxy_options.fAutoLogonIfChallenged = TRUE;
      if (WinHttpGetProxyForUrl(session.get(), wide_url.c_str(), &auto_proxy_options, &proxy_info))
        {
        if (!WinHttpSetOption(request.get(), WINHTTP_OPTION_PROXY, &proxy_info, sizeof(proxy_info)))
          {
          std::error_code error(GetLastError(), std::system_category());
          BOOST_THROW_EXCEPTION(std::system_error(error));
          }
        }

      if (!WinHttpSendRequest(request.get(), 0, 0, 0, 0, 0, 0))
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      if (!WinHttpReceiveResponse(request.get(), 0))
        {
        std::error_code error(GetLastError(), std::system_category());
        BOOST_THROW_EXCEPTION(std::system_error(error));
        }
      DWORD size = 0;
      DWORD down = 0;
      std::vector<char> buffer;
      do
        {
        size = 0;
        if (!WinHttpQueryDataAvailable(request.get(), &size))
          {
          std::error_code error(GetLastError(), std::system_category());
          BOOST_THROW_EXCEPTION(std::system_error(error));
          }
        buffer.resize(size);
        if (!WinHttpReadData(request.get(), buffer.data(), size, &down))
          {
          std::error_code error(GetLastError(), std::system_category());
          BOOST_THROW_EXCEPTION(std::system_error(error));
          }
        result.insert(result.end(), buffer.data(), buffer.data() + down);
        }
      while (down > 0);
      }
  private:
    HINTERNET get_connection(std::wstring const& host, short port)
      {
      for (const Connection& conn : connections)
        {
        if (conn.host == host && conn.port == port)
          {
          return conn.handle.get();
          }
        }
      Handle handle(
          WinHttpConnect(session.get(), host.c_str(), port, 0),
          WinHttpCloseHandle);
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
      return Handle(handle, WinHttpCloseHandle);
      }
  private:
    Handle session;
    struct Connection
      {
      Connection(std::wstring const& host, short port, Handle&& handle) :
          host(host), port(port), handle(std::move(handle))
        {
        }
      std::wstring host;
      short port;
      Handle handle;
      };
    std::vector<Connection> connections;
  };

} // namespace

namespace Karrot
{

std::vector<char> download(std::string const& url)
  {
  std::vector<char> result;
  if (boost::starts_with(url, "file://"))
    {
    std::ifstream stream(url.substr(7), std::ios::binary);
    if (!stream)
      {
      throw std::runtime_error("cannot open file '" + url + "'");
      }
    stream.unsetf(std::ios::skipws);
    stream.seekg(0, std::ios::end);
    std::size_t size = static_cast<std::size_t>(stream.tellg());
    stream.seekg(0);
    result.resize(size + 1);
    stream.read(result.data(), static_cast<std::streamsize>(size));
    result[size] = 0;
    }
  else
    {
    static Downloader downloader;
    downloader.download(url, result);
    }
  return result;
  }

} // namespace Karrot

#endif /* _WIN32 */
