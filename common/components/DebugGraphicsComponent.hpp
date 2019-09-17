#pragma once

#ifndef KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH
# define KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH 64
#endif

#include "string.hpp"
#include "Color.hpp"
#include "reflection/Reflectible.hpp"

namespace kengine {
	class DebugGraphicsComponent {
	public:
		static constexpr char stringName[] = "DebugGraphicsComponentString";
		using string = putils::string<KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH, stringName>;

		enum Type {
			Text,
			Line,
			Sphere,
			Box
		};

	public:
		DebugGraphicsComponent() = default;

		// Text
		DebugGraphicsComponent(const char * text, unsigned int textSize, const char * font, const putils::Rect3f & offset, const putils::NormalizedColor & color)
			: text(text), offset(offset), font(font), color(color), debugType(Text) {}

		// Sphere/Line/Box
		DebugGraphicsComponent(Type type, const putils::Rect3f & offset, const putils::NormalizedColor & color)
			: offset(offset), color(color), debugType(type) {}

		string text; 
		string font;

		putils::NormalizedColor color;
		putils::Rect3f offset{ {}, { 1.f, 1.f, 1.f } };
		// text size: offset.size.x
		// circle radius: offset.size.x
		// line thickness: offset.size.y

		Type debugType = Type::Box;

		pmeta_get_class_name(DebugGraphicsComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DebugGraphicsComponent::text),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::font),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::color),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::offset),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::debugType)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}