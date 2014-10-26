/*
 * Copyright (C) 2014 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "cache.hpp"
#include <karrot/spec.hpp>
#include "../src/driver.hpp"
#include <karrot/implementation.hpp>
#include "yaml_writer.hpp"
#include <boost/lexical_cast.hpp>
#include <memory>

namespace Karrot
{

void write_cache(std::string const& filename, std::vector<int> const& model,
        Database const& database)
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
        writer.scalar(impl.id);

        writer.scalar("name");
        writer.scalar(impl.name);

        writer.scalar("version");
        writer.scalar(impl.version);

        writer.scalar("component");
        writer.scalar(impl.component);

        writer.scalar("driver");
        writer.scalar(impl.driver->name());

        if (!impl.variant.empty())
        {
            writer.scalar("variant");
            writer.start_mapping();

            for (auto&& entry : impl.variant)
            {
                writer.scalar(entry.first);
                writer.scalar(entry.second);
            }

            writer.end_mapping();
        }

        if (!impl.values.empty())
        {
            writer.scalar("values");
            writer.start_mapping();

            for (auto&& entry : impl.values)
            {
                writer.scalar(entry.first);
                writer.scalar(entry.second);
            }

            writer.end_mapping();
        }

        if (!impl.depends.empty())
        {
            writer.scalar("depends");
            writer.start_sequence();

            for (auto&& dep : impl.depends)
            {
                writer.scalar(boost::lexical_cast<std::string>(dep));
            }

            writer.end_sequence();
        }

        if (!impl.conflicts.empty())
        {
            writer.scalar("conflicts");
            writer.start_sequence();

            for (auto&& dep : impl.conflicts)
            {
                writer.scalar(boost::lexical_cast<std::string>(dep));
            }

            writer.end_sequence();
        }

        writer.end_mapping();
    }

    writer.end_sequence();
}

} // namespace Karrot
