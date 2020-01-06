#pragma once

#include "Point.hpp"
#include "Entity.hpp"

namespace kengine {
	struct ViewportComponent {
		using RenderTexture = void *;

		Entity::ID window = Entity::INVALID_ID;
		putils::Rect2f boundingBox = { { 0.f, 0.f }, { 1.f, 1.f } };
		putils::Point2i resolution = { 1280, 720 };
		float zOrder = 1.f;
		RenderTexture renderTexture = (RenderTexture)-1;

		putils_reflection_class_name(ViewportComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&ViewportComponent::window),
			putils_reflection_attribute(&ViewportComponent::boundingBox),
			putils_reflection_attribute(&ViewportComponent::resolution),
			putils_reflection_attribute(&ViewportComponent::zOrder)
		);
	};
}