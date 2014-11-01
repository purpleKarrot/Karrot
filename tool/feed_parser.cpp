/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "feed_parser.hpp"
#include "engine.hpp"
#include <karrot.h>
#include "xml_reader.hpp"
#include "variants.hpp"
#include <boost/format.hpp>
#include "url.hpp"

namespace Karrot
{

static const std::string xmlns = "http://purplekarrot.net/2013/project";

FeedParser::FeedParser(int id, Engine& engine) :
    id(id),
    engine(engine),
    queue(engine.feed_queue)
  {
  }

std::string FeedParser::next_element(XmlReader& xml) const
  {
  while (xml.start_element())
    {
    if (xml.namespace_uri() == xmlns)
      {
      return xml.name();
      }
    std::clog << boost::format("skipping '{%1%}:%2%'.\n") % xml.namespace_uri() % xml.name();
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
  int feed_id = engine.string_pool.from_string(xml.attribute("href", xmlns).c_str());
  if (feed_id != id)
    {
    id = feed_id;
    queue.current_id(id);
    }
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
    parse_packages(xml);
    xml.skip();
    tag = next_element(xml);
    }
  if (!tag.empty())
    {
    std::clog << boost::format("element '%1%' not expected!!\n") % tag;
    }
  }

void FeedParser::parse_meta(XmlReader& xml)
  {
  while (xml.start_element())
    {
    int name = engine.string_pool.from_string(xml.name().c_str());
    int value = engine.string_pool.from_string(xml.content().c_str());
    meta.emplace(name, value);
    xml.skip();
    }
  }

void FeedParser::parse_variants(XmlReader& xml)
  {
  while (xml.start_element())
    {
    int name = engine.string_pool.from_string(xml.attribute("name", xmlns).c_str());
    int values = engine.string_pool.from_string(xml.attribute("values", xmlns).c_str());
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
      int version = engine.string_pool.from_string(xml.attribute("version", xmlns).c_str());
      int tag = engine.string_pool.from_string(xml.attribute("tag", xmlns).c_str());
      add_src_package(version, tag);
      releases.push_back(version);
      }
    xml.skip();
    }
  }

void FeedParser::add_src_package(int version, int tag)
  {
  Driver const *driver = this->engine.package_handler.get(vcs_type);
  if (!driver)
    {
    return;
    }
  Implementation impl(id);
  impl.version = version;
  impl.component = STR_SOURCE;
  for (auto const& e : this->meta)
    {
    impl.set(e.first, e.second);
    }
  impl.set(engine.string_pool.from_static_string("href"), engine.string_pool.from_string(vcs_href.c_str()));
  impl.set(engine.string_pool.from_static_string("driver"), engine.string_pool.from_string(driver->name().c_str()));
  if(tag)
    {
    impl.set(engine.string_pool.from_static_string("tag"), tag);
    }
  for (const Dependencies& component : components)
    {
    component.replay(impl, engine.string_pool);
    }
  this->engine.add(std::move(impl));
  }

void FeedParser::parse_components(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.name() == "component" && xml.namespace_uri() == xmlns)
      {
      components.emplace_back(this->queue, engine.string_pool.from_string(xml.attribute("name", xmlns).c_str()));
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
      depends.start_if(xml.attribute("test", xmlns), engine.string_pool);
      parse_depends(xml, depends);
      depends.end_if();
      }
    else if (name == "elseif")
      {
      depends.start_elseif(xml.attribute("test", xmlns), engine.string_pool);
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
      std::string dep = resolve_uri(engine.string_pool.to_string(id), xml.attribute("href", xmlns));
      depends.depends(Spec(dep.c_str(), engine.string_pool));
      }
    else if (name == "conflicts")
      {
      std::string dep = resolve_uri(engine.string_pool.to_string(id), xml.attribute("href", xmlns));
      depends.conflicts(Spec(dep.c_str(), engine.string_pool));
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
  auto driver_name = xml.attribute("type", xmlns);
  Implementation impl(this->id);
  for (auto const& e : this->meta)
    {
    impl.set(e.first, e.second);
    }
  impl.version = engine.string_pool.from_string(xml.attribute("version", xmlns).c_str());
  impl.component = engine.string_pool.from_string(xml.attribute("component", xmlns).c_str());
  impl.set(engine.string_pool.from_static_string("driver"), engine.string_pool.from_string(driver_name.c_str()));
//if (auto attr = xml.optional_attribute("variant", xmlns))
//  {
//  auto variant = parse_variant(*attr);
//  // TODO: evaluate global spec to see whether this variant is allowed
//  }
  for (auto& attr : xml.attributes())
    {
    if (attr.name.namespace_uri == xmlns)
      {
      impl.set(engine.string_pool.from_string(attr.name.local.c_str()), engine.string_pool.from_string(attr.value.c_str()));
      }
    }
  auto driver = engine.package_handler.get(driver_name);
  driver->filter(impl, [&](Implementation& impl, bool system)
    {
    impl.depends.clear();
    impl.conflicts.clear();
    if (!system)
      {
      if (std::find(begin(releases), end(releases), impl.version) == end(releases))
        {
        return;
        }
      for (const Dependencies& component : components)
        {
        component.replay(impl, engine.string_pool);
        }
      }
    this->engine.add(std::move(impl));
    });
  }

} // namespace Karrot
