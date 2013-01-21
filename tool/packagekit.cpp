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
#include <algorithm>

namespace Karrot
{

PackageKit::PackageKit()
  {
  g_type_init();
  self = karrot_package_kit_new(0);
  gchar *distro_id = karrot_package_kit_distro_id(self);
  distro_ = std::string(distro_id, std::strcspn(distro_id, ";"));
  g_free(distro_id);
  }

PackageKit::~PackageKit()
  {
  karrot_package_kit_unref(self);
  }

bool PackageKit::resolve(
    const std::string& name,
    bool& installed,
    std::string& package_id)
  {
  gboolean out_installed;
  gchar* out_package_id = nullptr;
  karrot_package_kit_resolve(self, name.c_str(), &out_installed, &out_package_id);
  if (!out_package_id)
    {
    return false;
    }
  installed = out_installed;
  package_id = out_package_id;

  return true;
  }

void PackageKit::install_queued()
  {
  if (packages.empty())
    {
    return;
    }
  std::vector<gchar*> list(packages.size());
  std::transform(packages.begin(), packages.end(), list.begin(),
    [](const std::string& string)
    {
    return (gchar*) string.c_str();
    });
  karrot_package_kit_install(self, list.data(), list.size());
  }

const char* PKDriver::namespace_uri() const
  {
  return "http://purplekarrot.net/2013/packagekit";
  }

Dictionary PKDriver::fields() const
  {
  Dictionary fields;
  fields.insert(std::make_pair("distro", "*"));
  fields.insert(std::make_pair("name", std::string()));
  return fields;
  }

int PKDriver::filter(const Dictionary& fields, Implementation& impl)
  {
  const std::string& p_distro = fields.at("distro");
  if (p_distro != package_kit.distro() && p_distro != "*")
    {
    return Driver::INCOMPATIBLE;
    }
  bool installed;
  std::string package_id;
  if (!package_kit.resolve(fields.at("name"), installed, package_id))
    {
    return Driver::INCOMPATIBLE;
    }
  const char* begin = std::strchr(package_id.c_str(), ';') + 1;
  const char* end = std::strchr(begin, ';');
  impl.version = std::string(begin, end);
  impl.values["packageid"] = package_id;
  if (installed)
    {
    return Driver::SYS_INSTALLED;
    }
  return Driver::SYS_AVAILABLE;
  }

void PKDriver::download(const Implementation& impl, bool requested)
  {
  package_kit.queue_package(impl.values.at("packageid"));
  }

} // namespace Karrot
