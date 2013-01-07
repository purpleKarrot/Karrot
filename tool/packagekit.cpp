/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "packagekit.hpp"
#include "packagekit_g.h"

#include <karrot/deliverable.hpp>
#include <karrot/quark.hpp>

#include <iostream>
#include <cstring>

namespace karrot
{

static const int ASTERISK = string_to_quark("*");
static const int DISTRO   = string_to_quark("distro");
static const int NAME     = string_to_quark("name");

PackageKit::PackageKit()
  {
  g_type_init();
  impl = karrot_package_kit_new(0);
  gchar *distro_id = karrot_package_kit_distro_id(impl);
  distro = string_to_quark(distro_id, std::strcspn(distro_id, ";"));
  g_free(distro_id);
  }

PackageKit::~PackageKit()
  {
  karrot_package_kit_unref(impl);
  }

int PackageKit::namespace_uri() const
  {
  static const int instance = string_to_quark("http://karrot.org/2012/packagekit");
  return instance;
  }

Driver::Fields PackageKit::fields() const
  {
  Fields fields;
  fields.insert(std::make_pair(DISTRO, ASTERISK));
  fields.insert(std::make_pair(NAME, 0));
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

int PackageKit::filter(const Fields& fields, Identification& id, int& href, int& hash)
  {
  int p_distro = fields.find(DISTRO)->second;
  if (p_distro == ASTERISK || p_distro == distro)
    {
    int p_name = fields.find(NAME)->second;
    OnPackageTarget target(id.version, href, this);
    karrot_package_kit_resolve(impl, quark_to_string(p_name), on_package, &target);
    return target.filter;
    }
  return Driver::INCOMPATIBLE;
  }

void PackageKit::download(const Deliverable& deliverable)
  {
  packages.push_back((gchar*) quark_to_string(deliverable.href));
  }

void PackageKit::finalize()
  {
  if (!packages.empty())
    {
    karrot_package_kit_install(impl, &packages[0], packages.size());
    }
  }

} // namespace karrot
