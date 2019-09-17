#pragma once

#include "Point.hpp"
#include "Color.hpp"

namespace kengine {
	struct HighlightComponent {
		putils::NormalizedColor color;
		float intensity = 1.f;

		pmeta_get_class_name(HighlightComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&HighlightComponent::color),
			pmeta_reflectible_attribute(&HighlightComponent::intensity)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}