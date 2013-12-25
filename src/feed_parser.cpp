/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "feed_parser.hpp"
#include "engine.hpp"
#include "implementation.hpp"
#include "xml_reader.hpp"
#include "variants.hpp"
#include "log.hpp"
#include "url.hpp"

namespace Karrot
{

static std::string const xmlns = "http://purplekarrot.net/2013/project";

static const String SOURCE {"SOURCE"};
static const String HREF   {"href"};
static const String TAG {"tag"};

FeedParser::FeedParser(Spec const& spec, KEngine& engine) :
    spec(spec),
    engine(engine),
    queue(engine.feed_queue)
  {
  }

std::string FeedParser::next_element(XmlReader& xml, LogFunct& log) const
  {
  while (xml.start_element())
    {
    if (xml.namespace_uri() == xmlns)
      {
      return xml.name();
      }
    Log(log, "skipping '{%1%}:%2%'.") % xml.namespace_uri() % xml.name();
    xml.skip();
    }
  return std::string();
  }

void FeedParser::parse(XmlReader& xml, LogFunct& log)
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
  std::string tag = next_element(xml, log);
  if (tag == "meta")
    {
    parse_meta(xml);
    xml.skip();
    tag = next_element(xml, log);
    }
  if (tag == "vcs")
    {
    vcs_type = xml.attribute("type", xmlns);
    vcs_href = xml.attribute("href", xmlns);
    xml.skip();
    tag = next_element(xml, log);
    }
  if (tag == "variants")
    {
    parse_variants(xml);
    xml.skip();
    tag = next_element(xml, log);
    }
  if (tag == "releases")
    {
    parse_releases(xml);
    xml.skip();
    tag = next_element(xml, log);
    }
  if (tag == "components")
    {
    parse_components(xml);
    xml.skip();
    tag = next_element(xml, log);
    }
  if (tag == "packages")
    {
    if (spec.component != SOURCE)
      {
      parse_packages(xml);
      }
    xml.skip();
    tag = next_element(xml, log);
    }
  if (!tag.empty())
    {
    Log(log, "element '%1%' not expected!!") % tag;
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
  Driver const *driver = this->engine.package_handler.get(vcs_type);
  if (!driver)
    {
    return;
    }
  KImplementation impl
    {
    spec.id,
    String{this->name},
    String{version},
    SOURCE,
    driver
    };
  impl.values[String{"href"}] = vcs_href;
  impl.meta = this->meta;
  if(tag)
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
    this->engine.database.push_back(impl);
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
  KImplementation impl{this->spec.id, String{this->name}};
  impl.meta = this->meta;
  impl.version = xml.attribute("version", xmlns);
  impl.component = xml.attribute("component", xmlns);
  impl.driver = engine.package_handler.get(xml.attribute("type", xmlns));
  if (auto attr = xml.optional_attribute("variant", xmlns))
    {
    impl.variant = parse_variant(*attr);
    }
  for (auto& attr : xml.attributes())
    {
    if (attr.name.namespace_uri == impl.driver->xmlns())
      {
      impl.values[String{attr.name.local}] = attr.value;
      }
    }
  impl.driver->filter(impl, [&](KImplementation& impl, bool system)
    {
    if (!spec.query.evaluate(impl.version, impl.variant))
      {
      return;
      }
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
        component.replay(impl);
        }
      }
    this->engine.database.push_back(impl);
    });
  }

} // namespace Karrot
