/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "packagekit.hpp"
#include "packagekit_g.h"

#include <karrot/implementation.hpp>
#include <karrot/quark.hpp>

#include <iostream>
#include <cstring>

namespace karrot
{

PackageKit::PackageKit()
  {
  g_type_init();
  impl = karrot_package_kit_new(0);
  gchar *distro_id = karrot_package_kit_distro_id(impl);
  distro = std::string(distro_id, std::strcspn(distro_id, ";"));
  g_free(distro_id);
  }

PackageKit::~PackageKit()
  {
  karrot_package_kit_unref(impl);
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
  OnPackageTarget(int& version, int& href, PackageKit* channel) :
      version(version), href(href), channel(channel), filter(Driver::INCOMPATIBLE)
    {
    }
  int& version;
  int& href;
  PackageKit* channel;
  int filter;
  };

static void on_package(const gchar* info, const gchar* package_id, void* user_data)
  {
  OnPackageTarget& target = *reinterpret_cast<OnPackageTarget*>(user_data);
  const char* begin = std::strchr(package_id, ';') + 1;
  const char* end = std::strchr(begin, ';');
  target.version = string_to_quark(begin, end - begin);
  target.href = string_to_quark(package_id, std::strlen(package_id));
  if (std::strcmp(info, "installed") == 0)
    {
    target.filter = Driver::SYS_INSTALLED;
    }
  else
    {
    target.filter = Driver::SYS_AVAILABLE;
    }
  }

int PackageKit::filter(const Dictionary& fields, Identification& id, int& href, int& hash)
  {
  const std::string& p_distro = fields.at("distro");
  if (p_distro == "*" || p_distro == distro)
    {
    const std::string& p_name = fields.at("name");
    OnPackageTarget target(id.version, href, this);
    karrot_package_kit_resolve(impl, p_name.c_str(), on_package, &target);
    return target.filter;
    }
  return Driver::INCOMPATIBLE;
  }

void PackageKit::download(const Implementation& impl)
  {
  packages.push_back((gchar*) quark_to_string(impl.href));
  }

void PackageKit::install()
  {
  if (!packages.empty())
    {
    karrot_package_kit_install(impl, &packages[0], packages.size());
    }
  }

} // namespace karrot
