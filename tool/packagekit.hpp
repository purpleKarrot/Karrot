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
    void install();
  private:
    const char* namespace_uri() const override;
    Dictionary fields() const override;
    int filter(const Dictionary& fields, Identification& id, int& href, int& hash) override;
    void download(const Implementation& impl) override;
  private:
    std::vector<char*> packages;
    KarrotPackageKit* impl;
    std::string distro;
  };

} // namespace karrot

#endif /* KARROT_PACKAGEKIT_HPP */
