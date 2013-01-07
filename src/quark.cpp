/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "quark_internal.hpp"
#include "hash.hpp"
#include <vector>
#include <cassert>

namespace karrot
{

static std::size_t str_hash(const char* str)
  {
  std::size_t r = 0;
  std::size_t c;
  while ((c = *(const unsigned char*) str++) != 0)
    {
    r += (r << 3) + c;
    }
  return r;
  }

static std::size_t str_hash(const char* str, std::size_t len)
  {
  std::size_t r = 0;
  std::size_t c;
  while (len-- && (c = *(const unsigned char*) str++) != 0)
    {
    r += (r << 3) + c;
    }
  return r;
  }

static std::size_t str_hash(const int* str)
  {
  std::size_t hash = 0;
  for (; *str; ++str)
    {
    hash ^= *str + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
  return hash;
  }

static std::size_t str_hash(const int* str, std::size_t len)
  {
  std::size_t hash = 0;
  for (std::size_t i = 0; i < len; ++i)
    {
    hash ^= str[i] + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
  return hash;
  }

template<typename C>
class QuarkImpl
  {
  public:
    static QuarkImpl& instance()
      {
      static QuarkImpl instance_;
      return instance_;
      }
    int to_quark(const C* str, std::size_t len)
      {
      int quark = 0;
      if (!len || !str || !*str)
        {
        return quark;
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
      while ((quark = hash.table[h]) != 0)
        {
        if (std::equal(str, str + len, &space[index[quark]])
            && !space[index[quark] + len])
          {
          break;
          }
        h = hash.next(h, hh);
        }
      if (quark != 0)
        {
        return quark;
        }
      /* generate next quark and save in table */
      quark = index.size();
      hash.table[h] = quark;
      index.push_back(space.size());
      space.insert(space.end(), str, str + len);
      space.push_back(C(0));
      return quark;
      }
    const C* to_string(int quark) const
      {
      assert(quark >= 0);
      return &space[index[quark]];
      }
  private:
    QuarkImpl() :
        index(1, 0), space(1, C(0))
      {
      }
  private:
    Hash hash;
    std::vector<std::size_t> index;
    std::vector<C> space;
  };

int string_to_quark(const char* str, std::size_t len)
  {
  return QuarkImpl<char>::instance().to_quark(str, len);
  }

const char* quark_to_string(int quark)
  {
  return QuarkImpl<char>::instance().to_string(quark);
  }

int array_to_quark(const int* str, std::size_t len)
  {
  return QuarkImpl<int>::instance().to_quark(str, len);
  }

const int* quark_to_array(int quark)
  {
  return QuarkImpl<int>::instance().to_string(quark);
  }

} // namespace karrot
