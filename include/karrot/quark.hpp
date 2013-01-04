/*
 * Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#ifndef KARROT_QUARK_HPP
#define KARROT_QUARK_HPP

#include <cstddef>

namespace karrot
{

class Quark
  {
    typedef void (Quark::*bool_type)() const;
    void avoid_bool_comparison() const
      {
      }
  public:
    Quark() :
        id(0)
      {
      }
    template<int N>
    Quark(const char (&str)[N])
      {
      init(str, N - 1);
      }
    Quark(const char* str, std::size_t len)
      {
      init(str, len);
      }
    const char* str() const;
  public:
    bool operator==(const Quark& other) const
      {
      return id == other.id;
      }
    bool operator!=(const Quark& other) const
      {
      return id != other.id;
      }
    bool operator<(const Quark& other) const
      {
      return id < other.id;
      }
    bool operator!() const
      {
      return id == 0;
      }
    operator bool_type() const
      {
      return id == 0 ? 0 : &Quark::avoid_bool_comparison;
      }
  private:
    void init(const char* str, std::size_t len);
    int id;
  };

} // namespace karrot

#endif /* KARROT_QUARK_HPP */
