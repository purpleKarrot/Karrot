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
#include "engine.hpp"
#include "dependencies.hpp"
#include "feed_queue.hpp"
#include "package_handler.hpp"
#include "package.hpp"

namespace Karrot
{

class XmlReader;

class FeedParser
  {
  public:
    FeedParser(int id, Engine& engine);
    void parse(XmlReader& xml);
  private:
    std::string next_element(XmlReader& xml) const;
    void parse_meta(XmlReader& xml);
    void parse_variants(XmlReader& xml);
    void parse_releases(XmlReader& xml);
    void parse_components(XmlReader& xml);
    void parse_depends(XmlReader& xml, Dependencies& depends);
    void parse_packages(XmlReader& xml);
    void parse_package(XmlReader& xml);
    void add_src_package(int version, int tag);
  private:
    int id;
    Engine& engine;
    FeedQueue queue;
    std::map<int, int> meta;
    std::map<int, int> variants;
    std::vector<int> releases;
    std::vector<Dependencies> components;
    std::string vcs_type;
    std::string vcs_href;
  };

} // namespace Karrot

#endif /* KARROT_FEED_PARSER_HPP */
