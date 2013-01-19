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
    const char* namespace_uri() const override;
    Dictionary fields() const override;
    int filter(const Dictionary& fields, Identification& id, int& href, int& hash) override;
    void download(const Implementation& impl) override;
  private:
    std::string machine;
    std::string sysname;
  };

} // namespace karrot

#endif /* KARROT_ARCHIVE_HPP */
