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
#include <cstring>
#include <vector>
#include <unordered_map>

namespace Karrot
{

// block size for string storage
static const std::size_t block_size = 1024 * 4;

struct StringLess
{
    bool operator()(const char* lhs, const char* rhs) const
    {
        return std::strcmp(lhs, rhs) == 0;
    }
};

struct StringHash
{
    std::size_t operator()(const char* s) const
    {
        std::size_t h = (std::size_t)(*s);

        if (h)
        {
            for (++s; *s; ++s)
            {
                h = (h << 5) - h + (std::size_t) * s;
            }
        }

        return h;
    }
};

struct StringPool::Implementation
{
    int from_string(const char* str, bool copy)
    {
        if (!str)
        {
            return 0;
        }

        if (!*str)
        {
            return 1;
        }

        auto iter = table_.find(str);
        if (iter == table_.end())
        {
            if (copy)
            {
                str = add(str);
            }

            strings_.push_back(str);
            iter = table_.insert({str, strings_.size() - 1}).first;
        }

        return iter->second;
    }

    const char* add(const char* str)
    {
        std::size_t len = std::strlen(str);
        char* result = allocate(len);
        return std::strncpy(result, str, len);
    }

    char* allocate(std::size_t length)
    {
        std::size_t full_length = length + 1;

        if (full_length > std::size_t(end_ - next_))
        {
            std::size_t size = std::max(block_size, full_length);
            pool_.push_back(static_cast<char*>(::operator new(size)));
            next_ = pool_.back();
            end_ = next_ + size;
        }

        char* result(next_);

        next_ += length;
        *next_++ = '\0';

        return result;
    }

    //
    std::vector<const char*> strings_{nullptr, ""};

    // hash table
    std::unordered_map<const char*, int, StringHash, StringLess> table_;

    // storage
    std::vector<char*> pool_;
    char* next_ = nullptr;
    char* end_ = nullptr;
};

StringPool::StringPool() :
        pimpl(new Implementation)
{
    from_static_string("(");
    from_static_string(")");
    from_static_string("<");
    from_static_string("<=");
    from_static_string(">");
    from_static_string(">=");
    from_static_string("!=");
    from_static_string("==");
    from_static_string("&&");
    from_static_string("||");
}

StringPool::~StringPool()
{
    for (char* ptr : pimpl->pool_)
    {
        delete ptr;
    }
}

int StringPool::from_string(const char* str)
{
    return pimpl->from_string(str, true);
}

int StringPool::from_static_string(const char* str)
{
    return pimpl->from_string(str, false);
}

const char* StringPool::to_string(int val) const
{
    return pimpl->strings_[val];
}

} // namespace Karrot
