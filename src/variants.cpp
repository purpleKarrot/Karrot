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
struct DictionaryGrammar: qi::grammar<Iterator, Dictionary()>
  {
  DictionaryGrammar() : DictionaryGrammar::base_type(dict)
    {
    dict = entry % ';';
    entry = key >> '=' >> value;
    key = qi::char_("a-zA-Z") >> *qi::char_("a-zA-Z_0-9");
    value = +qi::char_("a-zA-Z_0-9");
    }
  qi::rule<Iterator, Dictionary()> dict;
  qi::rule<Iterator, std::pair<String, String>()> entry;
  qi::rule<Iterator, std::string()> key, value;
  };

Dictionary parse_variant(const std::string& string)
  {
  Dictionary result;
  auto begin = string.begin();
  DictionaryGrammar<decltype(begin)> grammar;
  if (!qi::parse(begin, string.end(), grammar, result))
    {
    throw std::runtime_error("invalid variant: " + string);
    }
  return result;
  }

static void r_variants_recurse(
    const Dictionary::const_iterator& cur,
    const Dictionary::const_iterator& end,
    const Dictionary& dict,
    const std::function<void(Dictionary)>& func)
  {
  assert(cur != end);
  std::vector<String> values;
  split(values, cur->second.get(), boost::is_any_of(";"), boost::token_compress_on);
  for (String const& val : values)
    {
    Dictionary copy(dict);
    copy.emplace(cur->first, val);
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
    const Dictionary& variants,
    const std::function<void(Dictionary)>& func)
  {
  if (variants.empty())
    {
    func(Dictionary());
    }
  else
    {
    r_variants_recurse(variants.begin(), variants.end(), Dictionary(), func);
    }
  }

} // namespace Karrot
