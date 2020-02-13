#pragma once

#ifndef KENGINE_NAME_COMPONENT_MAX_LENGTH
# define KENGINE_NAME_COMPONENT_MAX_LENGTH 64
#endif

#include "string.hpp"

namespace kengine {
	struct NameComponent {
		static constexpr char stringName[] = "NameComponentString";
		using string = putils::string<KENGINE_NAME_COMPONENT_MAX_LENGTH, stringName>;
		string name;

		putils_reflection_class_name(NameComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&NameComponent::name)
		);
		putils_reflection_used_types(
			putils_reflection_type(string)
		);
	};
}
