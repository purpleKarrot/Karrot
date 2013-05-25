/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUERY_HPP
#define KARROT_QUERY_HPP

#include <karrot.h>
#include <iostream>
#include <vector>
#include "string.hpp"
#include <boost/flyweight/key_value.hpp>

namespace Karrot
{

class Query
  {
  public:
    struct Token
      {
      enum Id
        {
        FALSE         =   0,
        TRUE          =   1,
        IDENTIFIER    =   3,
        LPAREN        = - 1,
        RPAREN        = - 2,
        LESS          = - 3,
        LESS_EQUAL    = - 4,
        GREATER       = - 5,
        GREATER_EQUAL = - 6,
        NOT_EQUAL     = - 7,
        EQUAL         = - 8,
        AND           = - 9,
        OR            = -10,
        };
      Token() = default;
      explicit Token(Id id)
        : id{id}
        {
        }
      explicit Token(bool b)
        : id{b ? TRUE : FALSE}
        {
        }
      explicit Token(String const& value)
        : id{IDENTIFIER}, value{value}
        {
        }
      Id id;
      String value;
      };
  public:
    Query() = default;
    Query(std::string const& string)
      : implementation{string}
      {
      }
  public:
    bool empty() const
      {
      return implementation.get_key().empty();
      }
    bool evaluate(std::string const& version, const KDictionary& variants) const;
  private:
    friend std::ostream& operator<<(std::ostream &os, Query const& query)
      {
      return os << query.implementation.get_key();
      }
  private:
    struct Implementation
      {
      Implementation(std::string const& string);
      std::vector<Token> compiled;
      };
    boost::flyweight
      <
      boost::flyweights::key_value<std::string, Implementation>,
      boost::flyweights::no_locking,
      boost::flyweights::no_tracking
      > implementation;
  };

} // namespace Karrot

#endif /* KARROT_QUERY_HPP */
