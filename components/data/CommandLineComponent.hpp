#pragma once

#include <vector>
#include <string>
#include "reflection.hpp"

namespace kengine {
    struct CommandLineComponent {
        std::vector<std::string> arguments;
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