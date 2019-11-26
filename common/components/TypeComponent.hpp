#pragma once

#ifndef KENGINE_TYPE_COMPONENT_NAME_MAX_SIZE
# define KENGINE_TYPE_COMPONENT_NAME_MAX_SIZE 128
#endif

#include "string.hpp"

namespace kengine {
    struct TypeComponent {
        static constexpr char stringName[] = "TypeComponentName";
        putils::string<KENGINE_TYPE_COMPONENT_NAME_MAX_SIZE, stringName> name;

        putils_reflection_class_name(TypeComponent);
        putils_reflection_attributes(
            putils_reflection_attribute(&TypeComponent::name)
        );
    };
}