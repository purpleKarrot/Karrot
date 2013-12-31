/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_FEED_PARSER_HPP
#define KARROT_FEED_PARSER_HPP

#include <vector>
#include <karrot/engine.hpp>
#include "dependencies.hpp"
#include "feed_queue.hpp"
#include "package.hpp"
#include "types.hpp"

namespace Karrot
{

class XmlReader;

struct Driver
  {
  std::string name;
  std::string xmlns;
  Engine::Filter filter;
  };

class FeedParser
  {
  public:
    FeedParser(Spec const& spec, std::vector<Driver>& drivers,
               Database& database, FeedQueue& feed_queue);
    void parse(XmlReader& xml);
  private:
    Driver* get_driver(std::string const& name) const;
    std::string next_element(XmlReader& xml) const;
    void parse_meta(XmlReader& xml);
    void parse_variants(XmlReader& xml);
    void parse_releases(XmlReader& xml);
    void parse_components(XmlReader& xml);
    void parse_depends(XmlReader& xml, Dependencies& depends);
    void parse_packages(XmlReader& xml);
    void parse_package(XmlReader& xml);
    void add_src_package(std::string const& version, boost::optional<std::string> const& tag);
  private:
    Spec spec;
    std::vector<Driver>& drivers;
    Database& database;
    FeedPreQueue queue;
    std::string name;
    std::shared_ptr<Dictionary> meta = std::make_shared<Dictionary>();
    Dictionary variants;
    std::vector<std::string> releases;
    std::vector<Dependencies> components;
    std::string vcs_type;
    std::string vcs_href;
  };

} // namespace Karrot

#endif /* KARROT_FEED_PARSER_HPP */
