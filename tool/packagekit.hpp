/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_PACKAGEKIT_HPP
#define KARROT_PACKAGEKIT_HPP

#include <karrot/driver.hpp>
#include <vector>

typedef struct _KarrotPackageKit KarrotPackageKit;

namespace karrot
{

class PackageKit: public Driver
  {
  public:
    PackageKit();
    ~PackageKit();
  private:
    int namespace_uri() const;
    Fields fields() const;
    int filter(const Fields& fields, Identification& id, int& href, int& hash);
    void download(const Deliverable& deliverable);
    void finalize();
    //Type type() const
    //  {
    //  return INTERNAL;
    //  }
  private:
    std::vector<char*> packages;
    KarrotPackageKit* impl;
    int distro;
  };

} // namespace karrot

#endif /* KARROT_PACKAGEKIT_HPP */
