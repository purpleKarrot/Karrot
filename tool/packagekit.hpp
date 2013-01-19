/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_PACKAGEKIT_HPP
#define KARROT_PACKAGEKIT_HPP

#include <karrot.hpp>
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
    int filter(const Dictionary& fields, Implementation& impl) override;
    void download(const Implementation& impl, bool requested) override;
  private:
    std::vector<char*> packages;
    KarrotPackageKit* self;
    std::string distro;
  };

} // namespace karrot

#endif /* KARROT_PACKAGEKIT_HPP */
