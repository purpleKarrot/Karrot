/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_HASH_HPP
#define KARROT_HASH_HPP

#include <cstdlib>

namespace Karrot
{

class Hash
  {
  public:
    Hash() :
        mask(0), table(0)
      {
      }
    ~Hash()
      {
      std::free(table);
      }
    bool rehash_needed(int size)
      {
      size *= 2;
      if (size <= mask)
        {
        return false;
        }
      while (size & (size - 1))
        {
        size &= size - 1;
        }
      mask = size * 2 - 1;
      std::free(table);
      table = (int*) std::calloc(mask + 1, sizeof(int));
      return true;
      }
    std::size_t begin() const
      {
      return 7;
      }
    std::size_t next(std::size_t h, std::size_t& hh) const
      {
      return (h + hh++) & mask;
      }
    int mask;
    int* table;
  };

} // namespace Karrot

#endif /* KARROT_HASH_HPP */
