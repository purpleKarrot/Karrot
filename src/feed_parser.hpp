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
#include "dependencies.hpp"
//#include <karrot/variants.hpp>
#include "feed_queue.hpp"
#include "package_handler.hpp"
#include "package.hpp"

namespace karrot
{

typedef std::vector<Deliverable> Database;

class XmlReader;

class FeedParser
  {
  private:
    struct Release
      {
      Release(int version, int tag) :
          version(version), tag(tag)
        {
        }
      int version, tag;
      };
  public:
    FeedParser(FeedQueue& qq, Database& db, PackageHandler& ph);
    bool parse(const Url& url, XmlReader& xml);
  private:
    void parse_variants(XmlReader& xml);
    void parse_releases(XmlReader& xml);
    void parse_build(XmlReader& xml);
    void parse_runtime(XmlReader& xml);
    void parse_components(XmlReader& xml);
    void parse_depends(XmlReader& xml, Dependencies& depends);
    void parse_packages(XmlReader& xml, Package group);
    void parse_package_fields(XmlReader& xml, Package& group);
  private:
    int folder;
    std::vector<int> variants;
    std::vector<Release> releases;
    std::vector<Dependencies> components;
    Url url;
    FeedQueue& queue;
    Database& db;
    PackageHandler& ph;
    int repo_type;
    int repo_href;
  };

} // namespace karrot

#endif /* KARROT_FEED_PARSER_HPP */
