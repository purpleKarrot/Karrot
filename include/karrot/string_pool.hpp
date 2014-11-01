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

#ifndef KARROT_STRING_POOL_INCLUDED
#define KARROT_STRING_POOL_INCLUDED

#include <memory>

namespace Karrot
{

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

} // namespace Karrot

#endif /* FUSEL_STRING_POOL_INCLUDED */
