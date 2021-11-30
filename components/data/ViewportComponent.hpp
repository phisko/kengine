#pragma once

#include "Rect.hpp"
#include "impl/ID.hpp"
#include "OnScreenComponent.hpp"

namespace kengine {
	struct ViewportComponent : OnScreenComponent {
		using RenderTexture = void *;
		static constexpr auto INVALID_RENDER_TEXTURE = (RenderTexture)-1;

		putils::Rect2f boundingBox = { { 0.f, 0.f }, { 1.f, 1.f } };
		putils::Point2i resolution = { 1280, 720 };
		float zOrder = 1.f;
		EntityID window = INVALID_ID;
		RenderTexture renderTexture = INVALID_RENDER_TEXTURE;

		ViewportComponent() noexcept {
			coordinateType = CoordinateType::ScreenPercentage;
		}
	};
}

#define refltype kengine::ViewportComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(window),
		putils_reflection_attribute(boundingBox),
		putils_reflection_attribute(resolution),
		putils_reflection_attribute(zOrder)
	);
	putils_reflection_parents(
		putils_reflection_type(kengine::OnScreenComponent)
	);
};
#undef refltype