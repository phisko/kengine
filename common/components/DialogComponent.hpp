#pragma once

#include "reflection/Reflectible.hpp"

namespace kengine {
	struct DialogComponent {
		struct Option {
			std::string text;
			std::function<void()> onClick = nullptr;

			pmeta_get_class_name(DialogComponentOption);
			pmeta_get_attributes(
				pmeta_reflectible_attribute(&Option::text),
				pmeta_reflectible_attribute(&Option::onClick)
			);
			pmeta_get_methods();
			pmeta_get_parents();
		};

		std::string mainText;
		std::vector<Option> options;

		pmeta_get_class_name(DialogComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DialogComponent::mainText),
			pmeta_reflectible_attribute(&DialogComponent::options)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}