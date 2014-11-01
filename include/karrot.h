// Copyright (c) 2014, Daniel Pfeifer <daniel@pfeifer-mail.de>
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#ifndef KARROT_H_INCLUDED
#define KARROT_H_INCLUDED

#include <map>
#include <memory>
#include <vector>

namespace Karrot
{

class Spec;
struct Implementation;

using Requests = std::vector<Spec>;
using Database = std::vector<Implementation>;

static const int STR_NULL          =  0;
static const int STR_EMPTY         =  1;
static const int STR_LPAREN        =  2;
static const int STR_RPAREN        =  3;
static const int STR_LESS          =  4;
static const int STR_LESS_EQUAL    =  5;
static const int STR_GREATER       =  6;
static const int STR_GREATER_EQUAL =  7;
static const int STR_NOT_EQUAL     =  8;
static const int STR_EQUAL         =  9;
static const int STR_AND           = 10;
static const int STR_OR            = 11;
static const int STR_ANY           = 12;
static const int STR_SOURCE        = 13;
static const int STR_VERSION       = 14;

class StringPool
{
public:
    StringPool();
    ~StringPool();

    StringPool(StringPool const&) = delete;
    StringPool& operator=(StringPool const&) = delete;

    StringPool(StringPool&&) = default;
    StringPool& operator=(StringPool&&) = default;

    int from_string(const char* str);
    int from_static_string(const char* str);

    const char* to_string(int val) const;

private:
    struct Implementation;
    std::unique_ptr<Implementation> pimpl;
};

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

class Query
  {
  public:
    Query() = default;
    Query(std::string const& string, StringPool& pool);
  public:
    explicit operator bool() const
      {
      return !compiled.empty();
      }
    std::string to_string(StringPool const& pool) const;
    bool evaluate(int version, const Dictionary& variants, StringPool const& pool) const;
  private:
    friend bool operator!=(Query const& q1, Query const& q2)
      {
      return q1.compiled != q2.compiled;
      }
  private:
    std::vector<int> compiled;
  };

class Spec
  {
  public:
    Spec() = default;
    Spec(
      const std::string& id,
      const std::string& component,
      const std::string& query, StringPool& pool);
    Spec(char const* url, StringPool& pool);
    bool satisfies(const Implementation& impl, StringPool const& pool) const;
  public:
    int id = 0;
    int component = 0;
    Query query;
  };

struct Implementation
  {
  int id;
  int version;
  int component;
  Dictionary values;
  std::vector<Spec> depends;
  std::vector<Spec> conflicts;
  };

bool solve(
    Database const& database,
    Requests const& requests,
    std::vector<int>& model,
    StringPool& pool);

} // namespace Karrot

#endif /* KARROT_H_INCLUDED */
