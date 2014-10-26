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

#include <karrot/string_pool.hpp>
#include <boost/detail/lightweight_test.hpp>

int string_pool(int argc, char* argv[])
{
    Karrot::StringPool pool;

    BOOST_TEST_EQ(pool.from_string("foo"), 2);
    BOOST_TEST_EQ(pool.from_string("bar"), 3);
    BOOST_TEST_EQ(pool.from_string("cow"), 4);

    BOOST_TEST_EQ(pool.from_string(nullptr), 0);
    BOOST_TEST_EQ(pool.from_string(""), 1);
    BOOST_TEST_EQ(pool.from_string("foo"), 2);

    return boost::report_errors();
}
