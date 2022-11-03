#pragma once

// stl
#include <vector>
#include <string>

// reflection
#include "reflection.hpp"

namespace kengine {
    struct CommandLineComponent {
        std::vector<std::string_view> arguments;
    };
}

#define refltype kengine::CommandLineComponent
putils_reflection_info{
    putils_reflection_class_name;
    putils_reflection_attributes(
        putils_reflection_attribute(arguments)
    );
};
#undef refltype