#pragma once

// stl
#include <vector>
#include <string>

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
    struct command_line {
        std::vector<std::string_view> arguments;
    };
}

#define refltype kengine::data::command_line
putils_reflection_info{
    putils_reflection_class_name;
    putils_reflection_attributes(
        putils_reflection_attribute(arguments)
    );
};
#undef refltype