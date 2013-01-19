/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DRIVER_HPP
#define KARROT_DRIVER_HPP

#include <karrot/dictionary.hpp>

namespace karrot
{

class Identification;
class Implementation;

class Driver
  {
  public:
    virtual ~Driver();
  public:
    virtual const char* namespace_uri() const
      {
      return nullptr;
      }
    virtual Dictionary fields() const
      {
      return Dictionary();
      }
    virtual int filter(const Dictionary& fields, Identification& id, int& href, int& hash)
      {
      return 0;
      }
    virtual void download(const Implementation& implementation)
      {
      }
  public:
    static const int INCOMPATIBLE  = 0;
    static const int NORMAL        = 1;
    static const int SYS_INSTALLED = 2;
    static const int SYS_AVAILABLE = 3;
  };

} // namespace karrot

#endif /* KARROT_DRIVER_HPP */
