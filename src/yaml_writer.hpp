/*
 * Copyright (C) 2014 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include <string>
#include <yaml.h>

namespace Yaml
{

class Writer
{
public:
    Writer(FILE* file);
    ~Writer();

    Writer(Writer const&) = delete;
    Writer& operator=(Writer const&) = delete;

    void scalar(std::string const& val);

    void start_sequence(bool flow = false);
    void end_sequence();

    void start_mapping(bool flow = false);
    void end_mapping();

private:
    yaml_emitter_t emitter;
};

} // namespace Yaml
