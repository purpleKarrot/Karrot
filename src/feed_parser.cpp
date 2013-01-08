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
#include <karrot/deliverable.hpp>
#include <iostream>

namespace karrot
{

static const int NAMESPACE    = string_to_quark("http://ryppl.org/2012/feed");
static const int VCS          = string_to_quark("http://ryppl.org/2012/versioncontrol");

static const int BUILD        = string_to_quark("build");
static const int COMPONENT    = string_to_quark("component");
static const int COMPONENTS   = string_to_quark("components");
static const int CONFIG       = string_to_quark("config");
static const int CONFLICTS    = string_to_quark("conflicts");
static const int DATE         = string_to_quark("date");
static const int DEPENDENCIES = string_to_quark("dependencies");
static const int DEPENDS      = string_to_quark("depends");
static const int ELSE         = string_to_quark("else");
static const int ELSEIF       = string_to_quark("elseif");
static const int ENDIF        = string_to_quark("endif");
static const int GROUP        = string_to_quark("group");
static const int HREF         = string_to_quark("href");
static const int IF           = string_to_quark("if");
static const int META         = string_to_quark("meta");
static const int NAME         = string_to_quark("name");
static const int PACKAGE      = string_to_quark("package");
static const int PACKAGES     = string_to_quark("packages");
static const int PROJECT      = string_to_quark("project");
static const int RELEASE      = string_to_quark("release");
static const int RELEASES     = string_to_quark("releases");
static const int REPOSITORY   = string_to_quark("repository");
static const int RUNTIME      = string_to_quark("runtime");
static const int SUMMARY      = string_to_quark("summary");
static const int TAG          = string_to_quark("tag");
static const int TEST         = string_to_quark("test");
static const int TYPE         = string_to_quark("type");
static const int VALUES       = string_to_quark("values");
static const int VARIANT      = string_to_quark("variant");
static const int VARIANTS     = string_to_quark("variants");
static const int VERSION      = string_to_quark("version");

FeedParser::FeedParser(FeedQueue& queue, Database& db, PackageHandler& ph) :
    queue(queue),
    db(db),
    ph(ph)
  {
  }

static int next_element(XmlReader& xml)
  {
  while (xml.start_element())
    {
    int name = xml.name();
    int namespace_uri = xml.namespace_uri();
    if (namespace_uri == NAMESPACE)
      {
      return name;
      }
    printf("skipping %s:%s\n", quark_to_string(namespace_uri), quark_to_string(name));
    xml.skip();
    }
  return int();
  }

bool FeedParser::parse(const Url& url, XmlReader& xml)
  {
  this->url = url;
  if (xml.name() != PROJECT || xml.namespace_uri() != NAMESPACE)
    {
    printf("not a karrot feed\n");
    return false;
    }
  name = xml.attribute(NAME, NAMESPACE);
  if (!name)
    {
    printf("name missing!\n");
    return false;
    }
  //int href = xml.get_attribute (int.HREF, NAMESPACE);
  //stdout.printf ("href: %s\n", href.to_string());

  int tag = next_element(xml);
  if (tag == META)
    {
    // currently not interested in meta information!
    xml.skip();
    tag = next_element(xml);
    }
  else
    {
    printf("meta expected!!\n");
    }

  if (tag == VARIANTS)
    {
    parse_variants(xml);
    tag = next_element(xml);
    }
  if (tag == RELEASES)
    {
    parse_releases(xml);
    tag = next_element(xml);
    }
  if (tag == BUILD)
    {
    int type = xml.attribute(TYPE, VCS);
    int href = xml.attribute(HREF, VCS);
    parse_build(xml, type, href);
    tag = next_element(xml);
    }
  if (tag == RUNTIME)
    {
    parse_runtime(xml);
    tag = next_element(xml);
    }
  else if (tag == COMPONENTS)
    {
    parse_components(xml);
    tag = next_element(xml);
    }
  if (tag == PACKAGES)
    {
    Package group(this->url.host, this->url.path);
    parse_packages(xml, group);
    tag = next_element(xml);
    }
  if (tag)
    {
    printf("tag %s not expected!!\n", quark_to_string(tag));
    }
  return true;
  }

void FeedParser::parse_variants(XmlReader& xml)
  {
  while (xml.start_element())
    {
    variants.push_back(xml.attribute(NAME, NAMESPACE));
    variants.push_back(xml.attribute(VALUES, NAMESPACE));
    xml.skip();
    }
  }

void FeedParser::parse_releases(XmlReader& xml)
  {
  while (xml.start_element())
    {
    int name = xml.name();
    int namespace_uri = xml.namespace_uri();
    if (name == RELEASE && namespace_uri == NAMESPACE)
      {
      int version = xml.attribute(VERSION, NAMESPACE);
      int tag = xml.attribute(TAG, NAMESPACE);
      releases.emplace_back(version, tag);
      }
    xml.skip();
    }
  }

void FeedParser::parse_build(XmlReader& xml, int type, int href)
  {
  Dependencies depends(ASTERISK);
  parse_depends(xml, depends);
  Driver* driver = this->ph.get(type);
  if (!driver)
    {
    return;
    }
  Deliverable deliverable;
  deliverable.id.domain = url.host;
  deliverable.id.project = url.path;
  deliverable.id.component = ASTERISK;
  deliverable.name = name;
  deliverable.href = href;
  deliverable.driver = driver;
  for (std::size_t i = 0; i < releases.size(); ++i)
    {
    deliverable.id.version = releases[i].version;
    deliverable.hash = releases[i].tag;
    foreach_variant(variants, [&](int variant)
      {
      deliverable.id.variant = variant;
      deliverable.depends.clear();
      deliverable.conflicts.clear();
      depends.replay(ASTERISK, deliverable.id.version, variant,
          deliverable.depends, deliverable.conflicts);
      db.push_back(deliverable);
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
    int name = xml.name();
    int namespace_uri = xml.namespace_uri();
    if (name == COMPONENT && namespace_uri == NAMESPACE)
      {
      int name = xml.attribute(NAME, NAMESPACE);
      components.emplace_back(name);
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
    int name = xml.name();
    if (name == IF)
      {
      depends.start_if(xml.attribute(TEST, NAMESPACE));
      parse_depends(xml, depends);
      depends.end_if();
      }
    else if (name == ELSEIF)
      {
      depends.start_elseif(xml.attribute(TEST, NAMESPACE));
      parse_depends(xml, depends);
      depends.end_if();
      }
    else if (name == ELSE)
      {
      depends.start_else();
      parse_depends(xml, depends);
      depends.end_if();
      }
    else if (name == DEPENDS)
      {
      std::string href = quark_to_string(xml.attribute(HREF, NAMESPACE));
      Url dep(href.c_str(), &url);
      queue.push(dep);
      depends.depends(Spec(dep));
      xml.skip();
      }
    else if (name == CONFLICTS)
      {
      std::string href = quark_to_string(xml.attribute(HREF, NAMESPACE));
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
  typedef std::map<int, int>::const_iterator map_iter;
  for (map_iter it = package.fields.begin(); it != package.fields.end(); ++it)
    {
    if (!it->second)
      {
      std::cerr << "required attribute not set: " << it->first << std::endl;
      valid = false;
      }
    }
  return valid;
  }

void FeedParser::parse_packages(XmlReader& xml, Package group)
  {
  while (xml.start_element())
    {
    int name = xml.name();
    int namespace_uri = xml.namespace_uri();
    if (name == GROUP && namespace_uri == NAMESPACE)
      {
      parse_package_fields(xml, group);
      parse_packages(xml, group);
      }
    else if (name == PACKAGE && namespace_uri == NAMESPACE)
      {
      parse_package_fields(xml, group);
      if (package_is_valid(group))
        {
        Deliverable deliverable;
        deliverable.id = group.id;
        deliverable.name = this->name;
        int flags = group.driver->filter(group.fields, deliverable.id,
            deliverable.href, deliverable.hash);
        if (flags)
          {
          if ((flags | 0x1) != 0) // not INSTALLED
            {
            deliverable.driver = group.driver;
            }
          if ((flags | 0x2) == 0) // not SYSTEM
            {
            Identification& id = deliverable.id;
            for (const Dependencies& component : components)
              {
              component.replay(id.component, id.version, id.variant,
                  deliverable.depends, deliverable.conflicts);
              }
            }
          this->db.push_back(deliverable);
          }
        }
      else
        {
        std::cout << quark_to_string(name) << " is invalid!" << std::endl;
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
  if ((attr = xml.attribute(COMPONENT, NAMESPACE)))
    {
    group.id.component = attr;
    }
  if ((attr = xml.attribute(VERSION, NAMESPACE)))
    {
    group.id.version = attr;
    }
  if ((attr = xml.attribute(VARIANT, NAMESPACE)))
    {
    group.id.variant = parse_variant(attr);
    }
  if ((attr = xml.attribute(TYPE, NAMESPACE)))
    {
    group.driver = this->ph.get(attr);
    if (group.driver)
      {
      group.fields = group.driver->fields();
      }
    }
  if (group.driver)
    {
    int namespace_uri = group.driver->namespace_uri();
    typedef std::map<int, int>::iterator map_iter;
    for (map_iter it = group.fields.begin(); it != group.fields.end(); ++it)
      {
      if ((attr = xml.attribute(it->first, namespace_uri)))
        {
        it->second = attr;
        }
      }
    }
  }

} // namespace karrot
