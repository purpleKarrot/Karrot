/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "xml_reader.hpp"
#include "xml_re2c.hpp"
#include <karrot/quark.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/foreach.hpp>

namespace karrot
{

static const int XMLNS = string_to_quark("xmlns");

void XmlReader::lookup_namespace(Name& name)
  {
  BOOST_FOREACH(const Mapping& mapping, boost::adaptors::reverse(ns_mappings))
    {
    if (name.prefix == mapping.prefix)
      {
      name.namespace_uri = mapping.namespace_uri;
      return;
      }
    }
  name.namespace_uri = 0;
  }

std::size_t XmlReader::push_namespaces()
  {
  Mapping mapping;
  std::size_t previous_mappings = ns_mappings.size();
  std::vector<Attribute>::iterator begin = attributes.begin();
  std::vector<Attribute>::iterator end = attributes.end();
  std::vector<Attribute>::iterator attr;
  for (attr = begin; attr != end; ++attr)
    {
    if (!attr->name.prefix && attr->name.local == XMLNS)
      {
      mapping.prefix = 0;
      mapping.namespace_uri = attr->value;
      ns_mappings.push_back(mapping);
      }
    else if (attr->name.prefix == XMLNS)
      {
      mapping.prefix = attr->name.local;
      mapping.namespace_uri = attr->value;
      ns_mappings.push_back(mapping);
      }
    }
  lookup_namespace(current_name);
  for (attr = begin; attr != end; ++attr)
    {
    lookup_namespace(attr->name);
    }
  return previous_mappings;
  }

void XmlReader::pop_namespaces(std::size_t n)
  {
  ns_mappings.resize(n);
  }

void XmlReader::push_tag()
  {
  Tag tag;
  tag.previous_mappings = push_namespaces();
  tag.name = current_name;
  open_tags.push_back(tag);
  }

void XmlReader::pop_tag()
  {
  const Tag& tag = open_tags.back();
  pop_namespaces(tag.previous_mappings);
  current_name = tag.name;
  open_tags.pop_back();
  }

/******************************************************************************/

//#include "xml_re2c.h"

XmlReader::XmlReader(const boost::filesystem::path& filepath) :
    token_(token_none),
    skip_content(false),
    is_empty_element(false)
  {
  boost::filesystem::ifstream stream(filepath, std::ios::binary);
  if (!stream)
    {
    throw std::runtime_error("cannot open file " + filepath.string());
    }
  stream.unsetf(std::ios::skipws);
  stream.seekg(0, std::ios::end);
  std::size_t size = static_cast<std::size_t>(stream.tellg());
  stream.seekg(0);
  buffer.resize(size + 1);
  cursor = marker = buffer.begin();
  stream.read(&buffer[0], static_cast<std::streamsize>(size));
  buffer[size] = 0;
  }

XmlToken XmlReader::token() const
  {
  return token_;
  }

int XmlReader::name() const
  {
  return current_name.local;
  }

int XmlReader::namespace_uri() const
  {
  return current_name.namespace_uri;
  }

int XmlReader::attribute(int name, int namespace_uri) const
  {
  std::vector<Attribute>::const_iterator begin = attributes.begin();
  std::vector<Attribute>::const_iterator end = attributes.end();
  std::vector<Attribute>::const_iterator attr;
  for (attr = begin; attr != end; ++attr)
    {
    if (attr->name.local == name && attr->name.namespace_uri == namespace_uri)
      {
      return attr->value;
      }
    }
  return 0;
  }

void XmlReader::skip()
  {
  std::size_t depth = 0;
  if (is_empty_element)
    {
    is_empty_element = false;
    return;
    }
  skip_content = true;
  do
    {
    if ((token_ == token_element) && (!is_empty_element))
      {
      ++depth;
      }
    else if (token_ == token_end_element)
      {
      --depth;
      }
    }
  while (read() && depth > 0);
  skip_content = false;
  }

bool XmlReader::start_element()
  {
  if (!is_empty_element)
    {
    while (read())
      {
      if (token_ == token_element)
        {
        return true;
        }
      if (token_ == token_end_element)
        {
        return false;
        }
      }
    }
  is_empty_element = false;
  return false;
  }

std::string XmlReader::content()
  {
  int depth = 0;
  do
    {
    if ((token_ == token_element) && (!is_empty_element))
      {
      ++depth;
      }
    else if (token_ == token_end_element)
      {
      --depth;
      }
    }
  while (read() && depth > 0);
  return "text";
  }

} // namespace karrot
