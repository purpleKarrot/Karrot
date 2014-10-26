/*
 * Copyright (C) 2014 Daniel Pfeifer <daniel@pfeifer-mail.de>
 *
 * Distributed under the Boost Software License, Version 1.0.
 * See accompanying file LICENSE_1_0.txt or copy at
 *   http://www.boost.org/LICENSE_1_0.txt
 */

#include "yaml_writer.hpp"

namespace Yaml
{

Writer::Writer(FILE* file)
{
    yaml_emitter_initialize(&emitter);
    yaml_emitter_set_output_file(&emitter, file);
}

Writer::~Writer()
{
    yaml_emitter_flush(&emitter);
    yaml_emitter_delete(&emitter);
}

void Writer::scalar(std::string const& val)
{
    yaml_event_t event;
    yaml_scalar_event_initialize(&event, NULL, NULL,
            (yaml_char_t*) val.c_str(), -1, 1, 1, YAML_ANY_SCALAR_STYLE);
    yaml_emitter_emit(&emitter, &event);
}

void Writer::start_sequence(bool flow)
{
    yaml_event_t event;
    yaml_sequence_start_event_initialize(&event, NULL, NULL, 1,
            flow ? YAML_FLOW_SEQUENCE_STYLE : YAML_BLOCK_SEQUENCE_STYLE);
    yaml_emitter_emit(&emitter, &event);
}

void Writer::end_sequence()
{
    yaml_event_t event;
    yaml_sequence_end_event_initialize(&event);
    yaml_emitter_emit(&emitter, &event);
}

void Writer::start_mapping(bool flow)
{
    yaml_event_t event;
    yaml_mapping_start_event_initialize(&event, NULL, NULL, 1,
            flow ? YAML_FLOW_MAPPING_STYLE : YAML_BLOCK_MAPPING_STYLE);
    yaml_emitter_emit(&emitter, &event);
}

void Writer::end_mapping()
{
    yaml_event_t event;
    yaml_mapping_end_event_initialize(&event);
    yaml_emitter_emit(&emitter, &event);
}

} // namespace Yaml
