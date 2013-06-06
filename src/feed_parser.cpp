/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "feed_parser.hpp"
#include "engine.hpp"
#include "xml_reader.hpp"
#include "variants.hpp"
#include "log.hpp"
#include "url.hpp"

namespace Karrot
{

static const String SOURCE{"SOURCE"};

FeedParser::FeedParser(Spec const& spec, KEngine& engine) :
    spec(spec),
    queue(engine.feed_queue),
    engine(engine),
    project_ns{engine.namespace_uri + "project"}
  {
  }

std::string FeedParser::next_element(XmlReader& xml, KPrintFun log) const
  {
  while (xml.start_element())
    {
    if (xml.namespace_uri() == project_ns)
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
  if (xml.name() != "project" || xml.namespace_uri() != project_ns)
    {
    throw std::runtime_error("not a project feed");
    }
  std::string id = xml.attribute("href", project_ns);
  if (id != spec.id)
    {
    spec.id = id;
    queue.current_id(id);
    }
  name = xml.attribute("name", project_ns);
  std::string tag = next_element(xml, log);
  if (tag == "meta")
    {
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
    std::string vcs = xml.attribute("vcs", project_ns);
    std::string href = xml.attribute("href", project_ns);
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

void FeedParser::parse_variants(XmlReader& xml)
  {
  while (xml.start_element())
    {
    auto name = xml.attribute("name", project_ns);
    auto values = xml.attribute("values", project_ns);
    //variants.emplace(std::move(name), std::move(values));
    variants.insert(std::make_pair(std::move(name), std::move(values)));
    xml.skip();
    }
  }

void FeedParser::parse_releases(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.name() == "release" && xml.namespace_uri() == project_ns)
      {
      auto version = xml.attribute("version", project_ns);
      auto tag = xml.optional_attribute("tag", project_ns);
      releases.emplace_back(std::move(version), tag ? *tag : std::string());
      }
    xml.skip();
    }
  }

void FeedParser::parse_build(XmlReader& xml, const std::string& type, const std::string& href)
  {
  Dependencies depends(this->queue, "*");
  parse_depends(xml, depends);
  Driver const *driver = this->engine.package_handler.get(type);
  if (!driver)
    {
    return;
    }
  KImplementation impl(spec.id, this->name, SOURCE);
  impl.values["href"] = href;
  impl.driver = driver;
  for (std::size_t i = 0; i < releases.size(); ++i)
    {
    impl.version = releases[i].version();
    impl.values["tag"] = releases[i].tag();
    foreach_variant(variants, [&](KDictionary variant)
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
    if (xml.name() == "component" && xml.namespace_uri() == project_ns)
      {
      components.emplace_back(this->queue, xml.attribute("name", project_ns));
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
      depends.start_if(xml.attribute("test", project_ns));
      parse_depends(xml, depends);
      depends.end_if();
      }
    else if (name == "elseif")
      {
      depends.start_elseif(xml.attribute("test", project_ns));
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
      std::string dep = resolve_uri(spec.id, xml.attribute("href", project_ns));
      depends.depends(Spec(dep.c_str()));
      }
    else if (name == "conflicts")
      {
      std::string dep = resolve_uri(spec.id, xml.attribute("href", project_ns));
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
    if (name == "group" && namespace_uri == project_ns)
      {
      parse_package_fields(xml, group);
      parse_packages(xml, group);
      }
    else if (name == "package" && namespace_uri == project_ns)
      {
      parse_package_fields(xml, group);
      add_package(group);
      }
    xml.skip();
    }
  }

void FeedParser::parse_package_fields(XmlReader& xml, Package& group)
  {
  if (auto attr = xml.optional_attribute("component", project_ns))
    {
    group.component = std::move(*attr);
    }
  if (auto attr = xml.optional_attribute("version", project_ns))
    {
    group.version = std::move(*attr);
    }
  if (auto attr = xml.optional_attribute("variant", project_ns))
    {
    group.variant = parse_variant(*attr);
    }
  if (auto attr = xml.optional_attribute("type", project_ns))
    {
    group.driver = this->engine.package_handler.get(*attr);
    if (group.driver)
      {
      group.fields = group.driver->fields();
      }
    }
  if (group.driver)
    {
    std::string namespace_uri = group.driver->namespace_uri();
    for (auto& entry : group.fields)
      {
      if (auto attr = xml.optional_attribute(entry.first, namespace_uri))
        {
        entry.second = std::move(*attr);
        }
      }
    }
  }

static bool package_is_valid(const Package& package)
  {
  if (!package.driver)
    {
    return false;
    }
  bool valid = true;
  for (const auto& entry : package.fields)
    {
    if (entry.second.empty())
      {
      std::cerr << "required attribute not set: " << entry.first << std::endl;
      valid = false;
      }
    }
  return valid;
  }

void FeedParser::add_package(const Package& package)
  {
  if (!package_is_valid(package))
    {
    return;
    }
  package.driver->filter(package.fields,
    [&](DictView const& values, bool system)
    {
    KImplementation impl(
        spec.id,
        this->name,
        package.component,
        package.version,
        package.variant,
        package.values);
    impl.driver = package.driver;
    values.foreach([&impl](const std::string& key, const std::string& val)
      {
      if (key == "name")
        {
        impl.name = val;
        }
      else if (key == "component")
        {
        impl.component = val;
        }
      else if (key == "version")
        {
        impl.version = val;
        }
      else
        {
        impl.values[key] = val;
        }
      });
    if (!spec.query.evaluate(impl.version, impl.variant))
      {
      return;
      }
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
