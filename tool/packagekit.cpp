/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "packagekit.hpp"
#include "packagekit_g.h"

#include <iostream>
#include <cstring>

namespace Karrot
{

PackageKit::PackageKit()
  {
  g_type_init();
  self = karrot_package_kit_new(0);
  gchar *distro_id = karrot_package_kit_distro_id(self);
  distro = std::string(distro_id, std::strcspn(distro_id, ";"));
  g_free(distro_id);
  }

PackageKit::~PackageKit()
  {
  karrot_package_kit_unref(self);
  }

const char* PackageKit::namespace_uri() const
  {
  return "http://ryppl.org/2012/packagekit";
  }

Dictionary PackageKit::fields() const
  {
  Dictionary fields;
  fields.insert(std::make_pair("distro", "*"));
  fields.insert(std::make_pair("name", std::string()));
  return fields;
  }

struct OnPackageTarget
  {
  OnPackageTarget(Implementation& impl, PackageKit* channel) :
      impl(impl), channel(channel), filter(Driver::INCOMPATIBLE)
    {
    }
  Implementation& impl;
  PackageKit* channel;
  int filter;
  };

static void on_package(const gchar* info, const gchar* package_id, void* user_data)
  {
  OnPackageTarget& target = *reinterpret_cast<OnPackageTarget*>(user_data);
  const char* begin = std::strchr(package_id, ';') + 1;
  const char* end = std::strchr(begin, ';');
  target.impl.version = std::string(begin, end);
  target.impl.values["packageid"] = package_id;
  if (std::strcmp(info, "installed") == 0)
    {
    target.filter = Driver::SYS_INSTALLED;
    }
  else
    {
    target.filter = Driver::SYS_AVAILABLE;
    }
  }

int PackageKit::filter(const Dictionary& fields, Implementation& impl)
  {
  const std::string& p_distro = fields.at("distro");
  if (p_distro == "*" || p_distro == distro)
    {
    const std::string& p_name = fields.at("name");
    OnPackageTarget target(impl, this);
    karrot_package_kit_resolve(self, p_name.c_str(), on_package, &target);
    return target.filter;
    }
  return Driver::INCOMPATIBLE;
  }

void PackageKit::download(const Implementation& impl, bool requested)
  {
  packages.push_back((gchar*) impl.values.at("packageid").c_str());
  }

void PackageKit::install()
  {
  if (!packages.empty())
    {
    karrot_package_kit_install(self, &packages[0], packages.size());
    }
  }

} // namespace Karrot
