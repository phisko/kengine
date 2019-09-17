#pragma once

#ifndef KENGINE_NAME_COMPONENT_MAX_LENGTH
# define KENGINE_NAME_COMPONENT_MAX_LENGTH 64
#endif

#include "string.hpp"

namespace kengine {
	struct NameComponent {
		static constexpr char stringName[] = "NameComponentString";
		putils::string<KENGINE_NAME_COMPONENT_MAX_LENGTH, stringName> name;

		pmeta_get_class_name(NameComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&NameComponent::name)
		);
	};
}
