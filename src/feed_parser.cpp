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

static const String SOURCE {"SOURCE"};
static const String HREF   {"href"};
static const String TAG {"tag"};

FeedParser::FeedParser(Spec const& spec, KEngine& engine) :
    spec(spec),
    queue(engine.feed_queue),
    engine(engine)
  {
  }

std::string FeedParser::next_element(XmlReader& xml, KPrintFun log) const
  {
  while (xml.start_element())
    {
    if (xml.namespace_uri() == engine.xmlns)
      {
      return xml.name();
      }
    Log(log, "skipping '{%1%}:%2%'.") % xml.namespace_uri() % xml.name();
    xml.skip();
    }
  return std::string();
  }

void FeedParser::parse(XmlReader& xml, KPrintFun log)
  {
  if (xml.name() != "project" || xml.namespace_uri() != engine.xmlns)
    {
    throw std::runtime_error("not a project feed");
    }
  std::string id = xml.attribute("href", engine.xmlns);
  if (id != spec.id)
    {
    spec.id = id;
    queue.current_id(id);
    }
  name = xml.attribute("name", engine.xmlns);
  std::string tag = next_element(xml, log);
  if (tag == "meta")
    {
    parse_meta(xml);
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
  if (tag == "build")
    {
    std::string vcs = xml.attribute("vcs", engine.xmlns);
    std::string href = xml.attribute("href", engine.xmlns);
    parse_build(xml, vcs, href);
    xml.skip();
    tag = next_element(xml, log);
    }
  if (tag == "runtime")
    {
    if (spec.component != SOURCE)
      {
      parse_runtime(xml);
      }
    xml.skip();
    tag = next_element(xml, log);
    }
  else if (tag == "components")
    {
    if (spec.component != SOURCE)
      {
      parse_components(xml);
      }
    xml.skip();
    tag = next_element(xml, log);
    }
  if (tag == "packages")
    {
    if (spec.component != SOURCE)
      {
      Package group;
      parse_packages(xml, group);
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
    String name {xml.attribute("name", engine.xmlns)};
    String values {xml.attribute("values", engine.xmlns)};
    variants.emplace(name, values);
    xml.skip();
    }
  }

void FeedParser::parse_releases(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.name() == "release" && xml.namespace_uri() == engine.xmlns)
      {
      auto version = xml.attribute("version", engine.xmlns);
      auto tag = xml.optional_attribute("tag", engine.xmlns);
      releases.emplace_back(std::move(version), tag ? *tag : std::string());
      }
    xml.skip();
    }
  }

void FeedParser::parse_build(XmlReader& xml, const std::string& type, const std::string& href)
  {
  Driver const *driver = this->engine.package_handler.get(type);
  if (!driver)
    {
    return;
    }
  Dependencies depends(this->queue, "*");
  parse_depends(xml, depends);
  KImplementation impl
    {
    spec.id,
    String{this->name},
    String{},
    SOURCE,
    driver
    };
  impl.values[String{"href"}] = href;
  impl.meta = this->meta;
  impl.globals = &engine.globals;
  for (std::size_t i = 0; i < releases.size(); ++i)
    {
    impl.version = releases[i].version();
    impl.values[String{"tag"}] = releases[i].tag();
    foreach_variant(variants, [&](Dictionary variant)
      {
      if (!spec.query.evaluate(impl.version, variant))
        {
        return;
        }
      impl.variant = variant;
      impl.depends.clear();
      impl.conflicts.clear();
      depends.replay("*", impl.version, variant, impl.depends, impl.conflicts);
      this->engine.database.push_back(impl);
      });
    }
  }

void FeedParser::parse_runtime(XmlReader& xml)
  {
  components.emplace_back(this->queue);
  parse_depends(xml, components.back());
  }

void FeedParser::parse_components(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.name() == "component" && xml.namespace_uri() == engine.xmlns)
      {
      components.emplace_back(this->queue, xml.attribute("name", engine.xmlns));
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
      depends.start_if(xml.attribute("test", engine.xmlns));
      parse_depends(xml, depends);
      depends.end_if();
      }
    else if (name == "elseif")
      {
      depends.start_elseif(xml.attribute("test", engine.xmlns));
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
      std::string dep = resolve_uri(spec.id, xml.attribute("href", engine.xmlns));
      depends.depends(Spec(dep.c_str()));
      }
    else if (name == "conflicts")
      {
      std::string dep = resolve_uri(spec.id, xml.attribute("href", engine.xmlns));
      depends.conflicts(Spec(dep.c_str()));
      }
    xml.skip();
    }
  }

void FeedParser::parse_packages(XmlReader& xml, Package group)
  {
  while (xml.start_element())
    {
    std::string name = xml.name();
    std::string namespace_uri = xml.namespace_uri();
    if (name == "group" && namespace_uri == engine.xmlns)
      {
      parse_package_fields(xml, group);
      parse_packages(xml, group);
      }
    else if (name == "package" && namespace_uri == engine.xmlns)
      {
      parse_package_fields(xml, group);
      add_package(group);
      }
    xml.skip();
    }
  }

void FeedParser::parse_package_fields(XmlReader& xml, Package& group)
  {
  if (auto attr = xml.optional_attribute("component", engine.xmlns))
    {
    group.component = std::move(*attr);
    }
  if (auto attr = xml.optional_attribute("version", engine.xmlns))
    {
    group.version = std::move(*attr);
    }
  if (auto attr = xml.optional_attribute("variant", engine.xmlns))
    {
    group.variant = parse_variant(*attr);
    }
  if (auto attr = xml.optional_attribute("type", engine.xmlns))
    {
    group.driver = this->engine.package_handler.get(*attr);
    if (group.driver)
      {
      group.fields.clear();
      }
    }
  if (group.driver)
    {
    for (auto& attr : xml.attributes())
      {
      if (attr.name.namespace_uri == group.driver->xmlns())
        {
        group.fields[String{attr.name.local}] = attr.value;
        }
      }
    }
  }

void FeedParser::add_package(const Package& package)
  {
  KImplementation impl
    {
    spec.id,
    String{this->name},
    String{package.version},
    String{package.component},
    package.driver,
    package.variant,
    package.values,
    this->meta,
    &engine.globals
    };
  package.driver->filter(impl, [&](KImplementation& impl, bool system)
    {
    if (!spec.query.evaluate(impl.version, impl.variant))
      {
      return;
      }
    impl.depends.clear();
    impl.conflicts.clear();
    if (!system)
      {
      bool supported = std::any_of(begin(releases), end(releases),
        [&impl](const Release& release)
        {
        return impl.version == release.version();
        });
      if (!supported)
        {
        return;
        }
      for (const Dependencies& component : components)
        {
        component.replay(
            impl.component,
            impl.version,
            impl.variant,
            impl.depends,
            impl.conflicts);
        }
      }
    this->engine.database.push_back(impl);
    });
  }

} // namespace Karrot
