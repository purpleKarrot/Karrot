/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_MODULE_HPP
#define KARROT_MODULE_HPP

#include <karrot/string.hpp>
#include <karrot/dictionary.hpp>
#include <memory>

namespace Karrot
{

struct Module
{
  String id;
  String name;
  String version;
  String component;
  String driver;
  Dictionary variant;
  Dictionary values;
  std::shared_ptr<Dictionary> meta;
};

} // namespace Karrot

#endif /* KARROT_MODULE_HPP */
