#pragma once

#include "reflection.hpp"

namespace kengine {
    struct TimeModulatorComponent {
        float factor = 1.f;

        putils_reflection_class_name(TimeModulatorComponent);
        putils_reflection_attributes(
            putils_reflection_attribute(&TimeModulatorComponent::factor)
        );
    };
}