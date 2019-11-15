#pragma once

#include "Point.hpp"
#include "Entity.hpp"

namespace kengine {
	struct ViewportComponent {
		using RenderTexture = void *;

		Entity::ID window = Entity::INVALID_ID;
		putils::Rect2f boundingBox = { { 0.f, 0.f }, { 1.f, 1.f } };
		putils::Point2i resolution = { 1280, 720 };
		RenderTexture renderTexture = (RenderTexture)-1;

		pmeta_get_class_name(ViewportComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ViewportComponent::window),
			pmeta_reflectible_attribute(&ViewportComponent::boundingBox),
			pmeta_reflectible_attribute(&ViewportComponent::resolution)
		);
	};
}