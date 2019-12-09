#pragma once

#include "Point.hpp"
#include "Color.hpp"

namespace kengine {
	struct HighlightComponent {
		putils::NormalizedColor color;
		float intensity = 1.f;

		putils_reflection_class_name(HighlightComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&HighlightComponent::color),
			putils_reflection_attribute(&HighlightComponent::intensity)
		);
	};
}