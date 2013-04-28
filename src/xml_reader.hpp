/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_XML_READER_HPP
#define KARROT_XML_READER_HPP

#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <boost/spirit/home/support/iterators/line_pos_iterator.hpp>

namespace Karrot
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

class XmlReader
  {
  public:
    XmlReader(std::string const& filepath);
    bool read();
    XmlToken token() const;
    std::string name() const;
    std::string namespace_uri() const;
    std::string attribute(
      const std::string& name,
      const std::string& namespace_uri) const;
    boost::optional<std::string> optional_attribute(
      const std::string& name,
      const std::string& namespace_uri) const;
    void skip();
    bool start_element();
    std::string content();
  private:
    XmlReader(XmlReader const&) = delete;
    XmlReader& operator=(XmlReader const&) = delete;
  private:
    struct Name
      {
      std::string prefix;
      std::string local;
      std::string namespace_uri;
      };
    struct Tag
      {
      Name name;
      std::size_t previous_mappings;
      };
    struct Attribute
      {
      Name name;
      std::string value;
      };
    struct Mapping
      {
      std::string prefix;
      std::string namespace_uri;
      };
  private:
    void push_tag();
    void pop_tag();
    std::size_t push_namespaces();
    void pop_namespaces(std::size_t n);
    void lookup_namespace(Name& name);
    void throw_error(std::string const& message = std::string()) const;
  private:
    void parse_name(Name& name);
    void parse_attribute(Attribute& attribute);
    void parse_pi();
    void parse_comment();
    void parse_element();
    void parse_end_element();
    bool parse_text();
  private:
    std::vector<char> buffer;
    typedef std::vector<char>::iterator VectorIterator;
    typedef boost::spirit::line_pos_iterator<VectorIterator> Iterator;
    Iterator cursor;
    Iterator marker;
    XmlToken token_;
    Name current_name;
    std::vector<Attribute> attributes;
    std::vector<Mapping> ns_mappings;
    std::vector<Tag> open_tags;
    bool is_empty_element;
  };

} // namespace Karrot

#endif /* KARROT_XML_READER_HPP */
