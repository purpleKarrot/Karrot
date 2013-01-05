/*
 * Copyright (C) 2012 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_XML_READER_HPP
#define KARROT_XML_READER_HPP

#include <karrot/quark.hpp>
#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>
#include <string>
#include <vector>

namespace karrot
{

enum XmlToken
  {
  token_none,
  token_pi,
  token_element,
  token_end_element,
  token_text,
  token_cdata,
  token_comment
  };

class XmlReader: boost::noncopyable
  {
  public:
    XmlReader(const boost::filesystem::path& filepath);
    bool read();
    XmlToken token() const;
    Quark name() const;
    Quark namespace_uri() const;
    Quark attribute(Quark name, Quark namespace_uri) const;
    void skip();
    bool start_element();
    std::string content();
  private:
    struct Name
      {
      Quark prefix;
      Quark local;
      Quark namespace_uri;
      };
    struct Tag
      {
      Name name;
      std::size_t previous_mappings;
      };
    struct Attribute
      {
      Name name;
      Quark value;
      };
    struct Mapping
      {
      Quark prefix;
      Quark namespace_uri;
      };
  private:
    void push_tag();
    void pop_tag();
    std::size_t push_namespaces();
    void pop_namespaces(std::size_t n);
    void lookup_namespace(Name& name);
  private:
    bool parse_name(Name& name);
    bool parse_attribute(Attribute& attribute);
    bool parse_pi();
    bool parse_comment();
    bool parse_element();
    bool parse_end_element();
    bool parse_text();
  private:
    std::vector<char> buffer;
    std::vector<char>::iterator cursor;
    std::vector<char>::iterator marker;
    XmlToken token_;
    Name current_name;
    std::vector<Attribute> attributes;
    std::vector<Mapping> ns_mappings;
    std::vector<Tag> open_tags;
    bool is_empty_element;
    bool skip_content;
  };

} // namespace karrot

#endif /* KARROT_XML_READER_HPP */
