/*
 * Copyright (C) 2014 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "cache.hpp"
#include <karrot.h>
#include "yaml_writer.hpp"
#include <memory>

namespace Karrot
{

static std::string to_string(Spec const& spec, StringPool const& pool)
{
    std::string result = pool.to_string(spec.id);

    if (spec.component)
    {
        result += "#";
        result += pool.to_string(spec.component);
    }
    
    if (spec.query)
    {
        result += " ";
        result += spec.query.to_string(pool);
    }
    
    return result;
}

void write_cache(std::string const& filename, std::vector<int> const& model,
        Database const& database, StringPool const& pool)
{
    using File = std::unique_ptr<FILE, decltype(&fclose)>;
    File file(fopen(filename.c_str(), "w"), &fclose);
    Yaml::Writer writer(file.get());

    writer.start_sequence();

    for (int i : model)
    {
        writer.start_mapping();

        auto const& impl = database[i];

        writer.scalar("id");
        writer.scalar(pool.to_string(impl.id));

        writer.scalar("version");
        writer.scalar(pool.to_string(impl.version));

        writer.scalar("component");
        writer.scalar(pool.to_string(impl.component));

        impl.values.foreach([&writer, &pool](int key, int val)
        {
            writer.scalar(pool.to_string(key));
            writer.scalar(pool.to_string(val));
        });

        if (!impl.depends.empty())
        {
            writer.scalar("depends");
            writer.start_sequence();

            for (auto&& dep : impl.depends)
            {
                writer.scalar(to_string(dep, pool));
            }

            writer.end_sequence();
        }

        if (!impl.conflicts.empty())
        {
            writer.scalar("conflicts");
            writer.start_sequence();

            for (auto&& dep : impl.conflicts)
            {
                writer.scalar(to_string(dep, pool));
            }

            writer.end_sequence();
        }

        writer.end_mapping();
    }

    writer.end_sequence();
}

} // namespace Karrot
