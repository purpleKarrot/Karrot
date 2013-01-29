/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_ARCHIVE_HPP
#define KARROT_ARCHIVE_HPP

#include <karrot.hpp>
#include <string>

namespace Karrot
{

class Archive: public Driver
  {
  public:
    Archive();
  private:
    const char* namespace_uri() const; //override
    Fields fields() const; //override
    void filter(Dictionary const& fields, AddFun const& add); //override
    void download(const Implementation& impl, bool requested); //override
  private:
    std::string machine;
    std::string sysname;
  };

} // namespace Karrot

#endif /* KARROT_ARCHIVE_HPP */
