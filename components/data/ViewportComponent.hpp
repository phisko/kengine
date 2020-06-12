#pragma once

#include "Point.hpp"
#include "Entity.hpp"
#include "OnScreenComponent.hpp"

namespace kengine {
	struct ViewportComponent : OnScreenComponent {
		using RenderTexture = void *;

		putils::Rect2f boundingBox = { { 0.f, 0.f }, { 1.f, 1.f } };
		putils::Point2i resolution = { 1280, 720 };
		float zOrder = 1.f;
		Entity::ID window = Entity::INVALID_ID;
		RenderTexture renderTexture = (RenderTexture)-1;

		ViewportComponent() noexcept {
			coordinateType = CoordinateType::ScreenPercentage;
		}

		putils_reflection_class_name(ViewportComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&ViewportComponent::window),
			putils_reflection_attribute(&ViewportComponent::boundingBox),
			putils_reflection_attribute(&ViewportComponent::resolution),
			putils_reflection_attribute(&ViewportComponent::zOrder)
		);
		putils_reflection_parents(
			putils_reflection_type(OnScreenComponent)
		);
	};
}