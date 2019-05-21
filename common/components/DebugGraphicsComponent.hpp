#pragma once

#ifndef KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH
# define KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH 64
#endif

#define PUTILS_STRING_LENGTH KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH
#include "string.hpp"
#undef PUTILS_STRING_LENGTH

#include "reflection/Reflectible.hpp"

namespace kengine {
	class DebugGraphicsComponent {
	public:
		using string = putils::string<KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH>;

		enum Type {
			Text,
			Line,
			Sphere,
			Box
		};

	public:
		DebugGraphicsComponent() = default;

		// Text
		DebugGraphicsComponent(const char * text, unsigned int textSize, const char * font, const putils::Rect3f & offset, const putils::Point3f & colorNormalized)
			: text(text), offset(offset), font(font), colorNormalized(colorNormalized), debugType(Text) {}

		// Sphere/Line/Box
		DebugGraphicsComponent(Type type, const putils::Rect3f & offset, const putils::Point3f & colorNormalized)
			: offset(offset), colorNormalized(colorNormalized), debugType(type) {}

		string text; 
		string font;

		putils::Point3f colorNormalized;
		putils::Rect3f offset{ {}, { 1.f, 1.f, 1.f } };
		// text size: offset.size.x
		// circle radius: offset.size.x
		// line thickness: offset.size.y

		int debugType = Type::Box;

		pmeta_get_class_name(DebugGraphicsComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DebugGraphicsComponent::text),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::font),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::colorNormalized),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::offset),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::debugType)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}