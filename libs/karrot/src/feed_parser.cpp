/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "feed_parser.hpp"
#include "implementation.hpp"
#include "xml_reader.hpp"
#include "variants.hpp"
#include "url.hpp"

namespace Karrot
{

static std::string const xmlns = "http://purplekarrot.net/2013/project";

static const String SOURCE {"SOURCE"};
static const String HREF   {"href"};
static const String TAG    {"tag"};

FeedParser::FeedParser(Spec const& spec, std::vector<Driver>& drivers,
                       Database& database, FeedQueue& queue) :
    spec(spec),
    drivers(drivers),
    database(database),
    queue(queue)
  {
  }

Driver* FeedParser::get_driver(std::string const& name) const
  {
  auto found = std::find_if(drivers.begin(), drivers.end(),
      [&name](Driver const& driver)
      {
        return driver.name == name;
      });
  if (found == drivers.end())
    {
    return nullptr;
    }
  return &*found;
  }

std::string FeedParser::next_element(XmlReader& xml) const
  {
  while (xml.start_element())
    {
    if (xml.namespace_uri() == xmlns)
      {
      return xml.name();
      }
    xml.skip();
    }
  return std::string();
  }

void FeedParser::parse(XmlReader& xml)
  {
  if (xml.name() != "project" || xml.namespace_uri() != xmlns)
    {
    throw std::runtime_error("not a project feed");
    }
  std::string id = xml.attribute("href", xmlns);
  if (id != spec.id)
    {
    spec.id = id;
    queue.current_id(id);
    }
  name = xml.attribute("name", xmlns);
  std::string tag = next_element(xml);
  if (tag == "meta")
    {
    parse_meta(xml);
    xml.skip();
    tag = next_element(xml);
    }
  if (tag == "vcs")
    {
    vcs_type = xml.attribute("type", xmlns);
    vcs_href = xml.attribute("href", xmlns);
    xml.skip();
    tag = next_element(xml);
    }
  if (tag == "variants")
    {
    parse_variants(xml);
    xml.skip();
    tag = next_element(xml);
    }
  if (tag == "releases")
    {
    parse_releases(xml);
    xml.skip();
    tag = next_element(xml);
    }
  if (tag == "components")
    {
    parse_components(xml);
    xml.skip();
    tag = next_element(xml);
    }
  if (tag == "packages")
    {
    if (spec.component != SOURCE)
      {
      parse_packages(xml);
      }
    xml.skip();
    tag = next_element(xml);
    }
  if (!tag.empty())
    {
    std::clog << "element '" << tag << "' not expected!\n";
    }
  }

void FeedParser::parse_meta(XmlReader& xml)
  {
  while (xml.start_element())
    {
    String name {xml.name()};
    String value {xml.content()};
    meta->emplace(name, value);
    xml.skip();
    }
  }

void FeedParser::parse_variants(XmlReader& xml)
  {
  while (xml.start_element())
    {
    String name {xml.attribute("name", xmlns)};
    String values {xml.attribute("values", xmlns)};
    variants.emplace(name, values);
    xml.skip();
    }
  }

void FeedParser::parse_releases(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.name() == "release" && xml.namespace_uri() == xmlns)
      {
      auto version = xml.attribute("version", xmlns);
      auto tag = xml.optional_attribute("tag", xmlns);
      add_src_package(version, tag);
      releases.push_back(version);
      }
    xml.skip();
    }
  }

void FeedParser::add_src_package(std::string const& version, boost::optional<std::string> const& tag)
  {
  Driver* driver = get_driver(vcs_type);
  if (!driver)
    {
    return;
    }
  Implementation impl;
  impl.id = spec.id;
  impl.name = String{this->name};
  impl.version = String{version};
  impl.component = SOURCE;
  impl.driver = String{vcs_type};
  impl.values[String{"href"}] = vcs_href;
  impl.meta = this->meta;
  if (tag)
    {
    impl.values[String{"tag"}] = *tag;
    }
  foreach_variant(variants, [&](Dictionary variant)
    {
    if (!spec.query.evaluate(impl.version, variant))
      {
      return;
      }
    impl.variant = variant;
    impl.depends.clear();
    impl.conflicts.clear();
    for (const Dependencies& component : components)
      {
      component.replay(impl);
      }
    this->database.push_back(impl);
    });
  }

void FeedParser::parse_components(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.name() == "component" && xml.namespace_uri() == xmlns)
      {
      components.emplace_back(this->queue, xml.attribute("name", xmlns));
      parse_depends(xml, components.back());
      }
    xml.skip();
    }
  }

void FeedParser::parse_depends(XmlReader& xml, Dependencies& depends)
  {
  while (xml.start_element())
    {
    std::string name = xml.name();
    if (name == "if")
      {
      depends.start_if(xml.attribute("test", xmlns));
      parse_depends(xml, depends);
      depends.end_if();
      }
    else if (name == "elseif")
      {
      depends.start_elseif(xml.attribute("test", xmlns));
      parse_depends(xml, depends);
      depends.end_if();
      }
    else if (name == "else")
      {
      depends.start_else();
      parse_depends(xml, depends);
      depends.end_if();
      }
    else if (name == "depends")
      {
      std::string dep = resolve_uri(spec.id, xml.attribute("href", xmlns));
      depends.depends(Spec(dep.c_str()));
      }
    else if (name == "conflicts")
      {
      std::string dep = resolve_uri(spec.id, xml.attribute("href", xmlns));
      depends.conflicts(Spec(dep.c_str()));
      }
    xml.skip();
    }
  }

void FeedParser::parse_packages(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.name() == "package" && xml.namespace_uri() == xmlns)
      {
      parse_package(xml);
      }
    xml.skip();
    }
  }

void FeedParser::parse_package(XmlReader& xml)
  {
  Module module;
  module.id = this->spec.id;
  module.name = String{this->name};
  module.meta = this->meta;
  module.version = xml.attribute("version", xmlns);
  module.component = xml.attribute("component", xmlns);
  module.driver = xml.attribute("type", xmlns);
  Driver* driver = get_driver(module.driver);
  if (auto attr = xml.optional_attribute("variant", xmlns))
    {
    module.variant = parse_variant(*attr);
    }
  for (auto& attr : xml.attributes())
    {
    if (attr.name.namespace_uri == driver->xmlns)
      {
      module.values[String{attr.name.local}] = attr.value;
      }
    }
  driver->filter(std::move(module), [&](Module module, bool system)
    {
    if (!spec.query.evaluate(module.version, module.variant))
      {
      return;
      }
    Implementation impl{std::move(module)};
    if (!system)
      {
      if (std::find(begin(releases), end(releases), impl.version) == end(releases))
        {
        return;
        }
      for (const Dependencies& component : components)
        {
        component.replay(impl);
        }
      }
    this->database.push_back(std::move(impl));
    });
  }

} // namespace Karrot
