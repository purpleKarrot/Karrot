/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "feed_parser.hpp"
#include "xml_reader.hpp"
#include "variants.hpp"
#include <iostream>

namespace Karrot
{

static const std::string project_ns = "http://purplekarrot.net/2013/project";

FeedParser::FeedParser(FeedQueue& queue, Database& db, PackageHandler& ph) :
    queue(queue),
    db(db),
    ph(ph)
  {
  }

static std::string next_element(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.namespace_uri() == project_ns)
      {
      return xml.name();
      }
    std::cout << "skipping {" << xml.namespace_uri() << "}:" << xml.name() << std::endl;
    xml.skip();
    }
  return std::string();
  }

bool FeedParser::parse(const Url& url, XmlReader& xml)
  {
  this->url = url;
  if (xml.name() != "project" || xml.namespace_uri() != project_ns)
    {
    printf("not a karrot feed\n");
    return false;
    }
  name = to_quark(xml.attribute("name", project_ns));
  if (!name)
    {
    printf("name missing!\n");
    return false;
    }
  //int href = xml.get_attribute (int.HREF, feed_ns);
  //stdout.printf ("href: %s\n", href.to_string());

  std::string tag = next_element(xml);
  if (tag == "meta")
    {
    // currently not interested in meta information!
    xml.skip();
    tag = next_element(xml);
    }
  else
    {
    printf("meta expected!!\n");
    }

  if (tag == "variants")
    {
    parse_variants(xml);
    tag = next_element(xml);
    }
  if (tag == "releases")
    {
    parse_releases(xml);
    tag = next_element(xml);
    }
  if (tag == "build")
    {
    const std::string& vcs = xml.attribute("vcs", project_ns);
    const std::string& href = xml.attribute("href", project_ns);
    parse_build(xml, vcs, href);
    tag = next_element(xml);
    }
  if (tag == "runtime")
    {
    parse_runtime(xml);
    tag = next_element(xml);
    }
  else if (tag == "components")
    {
    parse_components(xml);
    tag = next_element(xml);
    }
  if (tag == "packages")
    {
    Package group;
    parse_packages(xml, group);
    tag = next_element(xml);
    }
  if (!tag.empty())
    {
    std::cerr << "element " << tag << " not expected!!" << std::endl;
    }
  return true;
  }

void FeedParser::parse_variants(XmlReader& xml)
  {
  while (xml.start_element())
    {
    variants.push_back(xml.attribute("name", project_ns));
    variants.push_back(xml.attribute("values", project_ns));
    xml.skip();
    }
  }

void FeedParser::parse_releases(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.name() == "release" && xml.namespace_uri() == project_ns)
      {
      const std::string& version = xml.attribute("version", project_ns);
      const std::string& tag = xml.attribute("tag", project_ns);
      releases.emplace_back(version, tag);
      }
    xml.skip();
    }
  }

void FeedParser::parse_build(XmlReader& xml, const std::string& type, const std::string& href)
  {
  Dependencies depends("*");
  parse_depends(xml, depends);
  Driver* driver = this->ph.get(type);
  if (!driver)
    {
    return;
    }
  DatabaseEntry entry(std::string(quark_to_string(url.host)) + quark_to_string(url.path));
  entry.impl.component = "SOURCE";
  entry.impl.name = name;
  entry.impl.values["href"] = href;
  entry.driver = driver;
  for (std::size_t i = 0; i < releases.size(); ++i)
    {
    entry.impl.version = releases[i].version();
    entry.impl.values["tag"] = releases[i].tag();
    foreach_variant(variants, [&](Dictionary variant)
      {
      entry.impl.variant = variant;
      entry.depends.clear();
      entry.conflicts.clear();
      depends.replay("*", entry.impl.version, variant,
          entry.depends, entry.conflicts);
      db.push_back(entry);
      });
    }
  }

void FeedParser::parse_runtime(XmlReader& xml)
  {
  components.emplace_back();
  parse_depends(xml, components.back());
  }

void FeedParser::parse_components(XmlReader& xml)
  {
  while (xml.start_element())
    {
    if (xml.name() == "component" && xml.namespace_uri() == project_ns)
      {
      components.emplace_back(xml.attribute("name", project_ns));
      parse_depends(xml, components.back());
      }
    else
      {
      xml.skip();
      }
    }
  }

void FeedParser::parse_depends(XmlReader& xml, Dependencies& depends)
  {
  while (xml.start_element())
    {
    const std::string& name = xml.name();
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
      const std::string& href = xml.attribute("href", project_ns);
      Url dep(href.c_str(), &url);
      queue.push(dep);
      depends.depends(Spec(dep));
      xml.skip();
      }
    else if (name == "conflicts")
      {
      const std::string& href = xml.attribute("href", project_ns);
      Url dep(href.c_str(), &url);
      depends.conflicts(Spec(dep));
      xml.skip();
      }
    else
      {
      xml.skip();
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

void FeedParser::parse_packages(XmlReader& xml, Package group)
  {
  while (xml.start_element())
    {
    const std::string& name = xml.name();
    const std::string& namespace_uri = xml.namespace_uri();
    if (name == "group" && namespace_uri == project_ns)
      {
      parse_package_fields(xml, group);
      parse_packages(xml, group);
      }
    else if (name == "package" && namespace_uri == project_ns)
      {
      parse_package_fields(xml, group);
      if (package_is_valid(group))
        {
        DatabaseEntry entry(std::string(quark_to_string(url.host)) + quark_to_string(url.path));
        entry.impl = group.impl;
        entry.impl.name = this->name;
        int flags = group.driver->filter(group.fields, entry.impl);
        if (flags)
          {
          if ((flags | 0x1) != 0) // not INSTALLED
            {
            entry.driver = group.driver;
            }
          if ((flags | 0x2) == 0) // not SYSTEM
            {
            //Identification& id = impl.id;
            for (const Dependencies& component : components)
              {
              component.replay(entry.impl.component, entry.impl.version,
                  entry.impl.variant, entry.depends, entry.conflicts);
              }
            }
          this->db.push_back(entry);
          }
        }
      else
        {
        std::cout << name << " is invalid!" << std::endl;
        }
      xml.skip();
      }
    else
      {
      xml.skip();
      }
    }
  }

void FeedParser::parse_package_fields(XmlReader& xml, Package& group)
  {
  int attr;
  if ((attr = to_quark(xml.attribute("component", project_ns))))
    {
    group.impl.component = attr;
    }
  if ((attr = to_quark(xml.attribute("version", project_ns))))
    {
    group.impl.version = attr;
    }
  if ((attr = to_quark(xml.attribute("variant", project_ns))))
    {
    group.impl.variant = parse_variant(quark_to_string(attr));
    }
  if ((attr = to_quark(xml.attribute("type", project_ns))))
    {
    group.driver = this->ph.get(quark_to_string(attr));
    if (group.driver)
      {
      group.fields = group.driver->fields();
      }
    }
  if (group.driver)
    {
    const char* namespace_uri = group.driver->namespace_uri();
    for (auto& entry : group.fields)
      {
      if ((attr = to_quark(xml.attribute(entry.first, namespace_uri))))
        {
        entry.second = attr;
        }
      }
    }
  }

} // namespace Karrot
