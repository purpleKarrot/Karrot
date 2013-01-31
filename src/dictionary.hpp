/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_DICTIONARY_HPP
#define KARROT_DICTIONARY_HPP

#include <karrot.h>
#include <cassert>
#include <string>
#include <map>

struct _KDictionary: std::map<std::string, std::string>
  {
  };

class DictView
  {
  public:
    DictView(char const **val, std::size_t size)
        : val(val), size(size)
      {
      assert(size % 2 == 0);
      }
    template<typename Function>
    void foreach(Function const& function) const
      {
      for (std::size_t i = 0; i < size; i += 2)
        {
        function(val[i], val[i + 1]);
        }
      }
  private:
    char const **val;
    std::size_t size;
  };

#endif /* KARROT_DICTIONARY_HPP */
