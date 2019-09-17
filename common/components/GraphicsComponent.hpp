#pragma once

#ifndef KENGINE_GRAPHICS_STRING_MAX_LENGTH
# define KENGINE_GRAPHICS_STRING_MAX_LENGTH 64
#endif

#include "string.hpp"
#include "Entity.hpp"
#include "Color.hpp"

namespace kengine {
    struct GraphicsComponent {
		static constexpr char stringName[] = "GraphicsComponentString";
		using string = putils::string<KENGINE_GRAPHICS_STRING_MAX_LENGTH, stringName>;

		string appearance;
		kengine::Entity::ID model = kengine::Entity::INVALID_ID; // Entity which had a ModelComponent
		putils::NormalizedColor color = { 1.f, 1.f, 1.f, 1.f };

        pmeta_get_class_name(GraphicsComponent);
        pmeta_get_attributes(
				pmeta_reflectible_attribute(&GraphicsComponent::appearance),
				pmeta_reflectible_attribute(&GraphicsComponent::model),
				pmeta_reflectible_attribute(&GraphicsComponent::color)
        );
		pmeta_get_parents();
    };
}
