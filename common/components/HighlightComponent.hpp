#pragma once

#include "Point.hpp"

namespace kengine {
	struct HighlightComponent {
		putils::Vector3f colorNormalized;
		float intensity = .5f;

		pmeta_get_class_name(HighlightComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&HighlightComponent::colorNormalized),
			pmeta_reflectible_attribute(&HighlightComponent::intensity)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}