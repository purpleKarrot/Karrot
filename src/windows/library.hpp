/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_WINDOWS_LIBRARY_HPP
#define KARROT_WINDOWS_LIBRARY_HPP

#include <windows.h>

namespace Karrot
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

} // namespace Karrot

#endif /* KARROT_WINDOWS_LIBRARY_HPP */
