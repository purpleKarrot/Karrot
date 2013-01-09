/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DRIVER_HPP
#define KARROT_DRIVER_HPP

#include <map>
#include <karrot/export.hpp>

namespace karrot
{

class Deliverable;
class Identification;

class KARROT_DECL Driver
  {
  public:
    typedef std::map<int, int> Fields;
    virtual ~Driver();
  public:
    virtual int namespace_uri() const
      {
      return 0;
      }
    virtual Fields fields() const
      {
      return Fields();
      }
    virtual int filter(const Fields& fields, Identification& id, int& href, int& hash)
      {
      return 0;
      }
    virtual void download(const Deliverable& deliverable)
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
