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

#include <karrot.h>
#include <algorithm>

namespace Karrot
{

bool solve(
    Database const& database,
    Requests const& requests,
    std::vector<int>& model,
    StringPool const& pool);

void Context::add(Implementation impl)
{
    database.push_back(std::move(impl));
}

void Context::erase(Spec const& spec, StringPool const& pool)
{
    auto it = std::remove_if(begin(database), end(database),
        [&spec, &pool](Implementation const& impl)
        {
            return spec.satisfies(impl, pool);
        });
    database.erase(it, end(database));
}

bool Context::solve(Requests const& requests, StringPool const& pool)
{
    return Karrot::solve(database, requests, model, pool);
}

void Context::for_each(std::function<void(Implementation const&)> fun) const
{
    for (auto& e : database)
    {
        fun(e);
    }
}

void Context::for_solution(std::function<void(Implementation const&)> fun) const
{
    for (int i : model)
    {
        fun(database[i]);
    }
}

} // namespace Karrot
