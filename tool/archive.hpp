/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_ARCHIVE_HPP
#define KARROT_ARCHIVE_HPP

#include <karrot/driver.hpp>

namespace karrot
{

class Archive: public Driver
  {
  public:
    Archive();
  private:
    int namespace_uri() const;
    Fields fields() const;
    int filter(const Fields& fields, Identification& id, int& href, int& hash);
    void download(const Implementation& impl);
    //Type type() const
    //  {
    //  return BINARY;
    //  }
  private:
    int machine;
    int sysname;
  };

} // namespace karrot

#endif /* KARROT_ARCHIVE_HPP */
