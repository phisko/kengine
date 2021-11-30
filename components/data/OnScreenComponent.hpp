#pragma once

namespace kengine {
	struct OnScreenComponent {
		enum class CoordinateType {
			ScreenPercentage,
			Pixels
		};

		CoordinateType coordinateType = CoordinateType::Pixels;
	};
}

#define refltype kengine::OnScreenComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(coordinateType)
	);
};
#undef refltype
