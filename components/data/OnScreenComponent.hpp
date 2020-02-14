#pragma once

namespace kengine {
	struct OnScreenComponent {
		enum class CoordinateType {
			ScreenPercentage,
			Pixels
		};

		CoordinateType coordinateType = CoordinateType::Pixels;

		putils_reflection_class_name(OnScreenComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&OnScreenComponent::coordinateType)
		);
	};
}
