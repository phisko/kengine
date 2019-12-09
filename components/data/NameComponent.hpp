#pragma once

#ifndef KENGINE_NAME_COMPONENT_MAX_LENGTH
# define KENGINE_NAME_COMPONENT_MAX_LENGTH 64
#endif

#include "string.hpp"

namespace kengine {
	struct NameComponent {
		static constexpr char stringName[] = "NameComponentString";
		putils::string<KENGINE_NAME_COMPONENT_MAX_LENGTH, stringName> name;

		putils_reflection_class_name(NameComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&NameComponent::name)
		);
	};
}
