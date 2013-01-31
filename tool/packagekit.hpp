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
#include <string>
#include <vector>

typedef struct _KarrotPackageKit KarrotPackageKit;

namespace Karrot
{

class PackageKit
  {
  public:
    PackageKit();
    ~PackageKit();
    const std::string& distro() const
      {
      return distro_;
      }
    bool resolve(
        const std::string& name,
        bool& installed,
        std::string& package_id);
    void queue_package(const std::string& package_id)
      {
      packages.push_back(package_id);
      }
    void install_queued();
  private:
    std::vector<std::string> packages;
    KarrotPackageKit* self;
    std::string distro_;
  };

class PKDriver: public Driver
  {
  public:
    PKDriver(PackageKit& package_kit)
        : package_kit(package_kit)
      {
      }
  private:
    const char* namespace_uri() const; //override
    void fields(Fields& out) const; //override
    void filter(Dictionary const& fields, AddFun const& add); //override
    void download(const Implementation& impl, bool requested); //override
  private:
    PackageKit& package_kit;
  };

} // namespace Karrot

#endif /* KARROT_PACKAGEKIT_HPP */
