/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DICTIONARY_HPP
#define KARROT_DICTIONARY_HPP

#include <map>

namespace Karrot
{

class Dictionary
{
public:
  void set(int key, int val)
    {
    impl[key] = val;
    }
  int get(int key) const
    {
    auto it = impl.find(key);
    return it != impl.end() ? it->second : 0;
    }
  template<typename Visit>
  void foreach(Visit&& visit) const
    {
    for(auto& e : impl)
      {
      visit(e.first, e.second);
      }
    }
private:
  std::map<int, int> impl;
};

} // namespace Karrot

#endif /* KARROT_DICTIONARY_HPP */
