/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUARK_IMPL_HPP
#define KARROT_QUARK_IMPL_HPP

#include "hash.hpp"
#include <vector>
#include <cstdlib>
#include <cassert>

namespace karrot
{

template<typename C>
class QuarkImpl
  {
  public:
    QuarkImpl() :
        index(1, 0), space(1, C(0))
      {
      }
    int to_int(const C* str, std::size_t len)
      {
      int id = 0;
      if (!len || !str || !*str)
        {
        return id;
        }
      /* expand hashtable if needed */
      if (hash.rehash_needed(index.size()))
        {
        for (std::size_t i = 1; i < index.size(); ++i)
          {
          std::size_t h = str_hash(&space[index[i]]) & hash.mask;
          std::size_t hh = hash.begin();
          while (hash.table[h] != 0)
            {
            h = hash.next(h, hh);
            }
          hash.table[h] = i;
          }
        }
      /* compute hash and check for match */
      std::size_t h = str_hash(str, len) & hash.mask;
      std::size_t hh = hash.begin();
      while ((id = hash.table[h]) != 0)
        {
        if (std::equal(str, str + len, &space[index[id]]) && !space[index[id] + len])
          {
          break;
          }
        h = hash.next(h, hh);
        }
      if (id != 0)
        {
        return id;
        }
      /* generate next id and save in table */
      id = index.size();
      hash.table[h] = id;
      index.push_back(space.size());
      space.insert(space.end(), str, str + len);
      space.push_back(C(0));
      return id;
      }
    const C* to_str(int id) const
      {
      assert(id >= 0);
      return &space[index[id]];
      }
  private:
    Hash hash;
    std::vector<std::size_t> index;
    std::vector<C> space;
  };

} // namespace karrot

#endif /* KARROT_QUARK_IMPL_HPP */
