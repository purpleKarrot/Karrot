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
#include <karrot/string.hpp>
#include <karrot/dictionary.hpp>
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
        False        =   0,
        True         =   1,
        Identifier   =   3,
        LParen       = - 1,
        RParen       = - 2,
        Less         = - 3,
        LessEqual    = - 4,
        Greater      = - 5,
        GreaterEqual = - 6,
        NotEqual     = - 7,
        Equal        = - 8,
        And          = - 9,
        Or           = -10,
        };
      Token() = default;
      explicit Token(Id id)
        : id{id}
        {
        }
      explicit Token(bool b)
        : id{b ? True : False}
        {
        }
      explicit Token(String const& value)
        : id{Identifier}, value{value}
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
    bool evaluate(std::string const& version, const Dictionary& variants) const;
  private:
    friend std::ostream& operator<<(std::ostream &os, Query const& query)
      {
      return os << query.implementation.get_key();
      }
    friend bool operator!=(Query const& q1, Query const& q2)
      {
      return q1.implementation != q2.implementation;
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
