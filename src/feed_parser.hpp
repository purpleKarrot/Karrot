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
#include "database.hpp"
#include "dependencies.hpp"
#include "feed_queue.hpp"
#include "package_handler.hpp"
#include "package.hpp"

namespace Karrot
{

class XmlReader;

class FeedParser
  {
  private:
    class Release
      {
      public:
        Release(const std::string& version, const std::string& tag)
            : version_(version), tag_(tag)
          {
          }
        const std::string& version() const
          {
          return version_;
          }
        const std::string& tag() const
          {
          return tag_.empty() ? version_ : tag_;
          }
      private:
        std::string version_, tag_;
      };
  public:
    FeedParser(Spec& spec, FeedQueue& qq, Database& db, PackageHandler& ph, std::string project_ns);
    void parse(XmlReader& xml, KPrintFun log);
  private:
    std::string next_element(XmlReader& xml, KPrintFun log) const;
    void parse_variants(XmlReader& xml);
    void parse_releases(XmlReader& xml);
    void parse_build(XmlReader& xml, const std::string& type, const std::string& href);
    void parse_runtime(XmlReader& xml);
    void parse_components(XmlReader& xml);
    void parse_depends(XmlReader& xml, Dependencies& depends);
    void parse_packages(XmlReader& xml, Package group);
    void parse_package_fields(XmlReader& xml, Package& group);
    void add_package(const Package& package);
  private:
    Spec& spec;
    std::string name;
    KDictionary variants;
    std::vector<Release> releases;
    std::vector<Dependencies> components;
    FeedQueue& queue;
    Database& db;
    PackageHandler& ph;
    std::string project_ns;
  };

} // namespace Karrot

#endif /* KARROT_FEED_PARSER_HPP */
