/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "variants.hpp"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/spirit/include/qi_nonterminal.hpp>
#include <boost/spirit/include/qi_operator.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/spirit/include/qi_char.hpp>

namespace Karrot
{

namespace qi = boost::spirit::qi;

template<typename Iterator>
struct DictionaryGrammar: qi::grammar<Iterator, KDictionary()>
  {
  DictionaryGrammar() : DictionaryGrammar::base_type(dict)
    {
    dict = entry % ';';
    entry = key >> '=' >> value;
    key = qi::char_("a-zA-Z") >> *qi::char_("a-zA-Z_0-9");
    value = +qi::char_("a-zA-Z_0-9");
    }
  qi::rule<Iterator, KDictionary()> dict;
  qi::rule<Iterator, std::pair<std::string, std::string>()> entry;
  qi::rule<Iterator, std::string()> key, value;
  };

KDictionary parse_variant(const std::string& string)
  {
  KDictionary result;
  auto begin = string.begin();
  DictionaryGrammar<decltype(begin)> grammar;
  if (!qi::parse(begin, string.end(), grammar, result))
    {
    throw std::runtime_error("invalid variant: " + string);
    }
  return result;
  }

static void r_variants_recurse(
    const KDictionary::const_iterator& cur,
    const KDictionary::const_iterator& end,
    const KDictionary& dict,
    const std::function<void(KDictionary)>& func)
  {
  assert(cur != end);
  std::vector<std::string> values;
  split(values, cur->second, boost::is_any_of(";"), boost::token_compress_on);
  for (const std::string& val : values)
    {
    KDictionary copy(dict);
    copy.insert(std::make_pair(cur->first, val));
    if (std::next(cur) == end)
      {
      func(copy);
      }
    else
      {
      r_variants_recurse(std::next(cur), end, copy, func);
      }
    }
  }

void foreach_variant(
    const KDictionary& variants,
    const std::function<void(KDictionary)>& func)
  {
  if (variants.empty())
    {
    func(KDictionary());
    }
  else
    {
    r_variants_recurse(variants.begin(), variants.end(), KDictionary(), func);
    }
  }

} // namespace Karrot
