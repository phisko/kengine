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

	public:
		DebugGraphicsComponent() = default;

		DebugGraphicsComponent(const char * text, unsigned int textSize, const char * font, const putils::Point3f & startPos, unsigned int color)
			: text(text), textSize(textSize), font(font), startPos(startPos), color(color), debugType(Text) {}

		DebugGraphicsComponent(const putils::Point3f & startPos, const putils::Point3f & endPos, float thickness, unsigned int color)
			: startPos(startPos), endPos(endPos), thickness(thickness), color(color), debugType(Line) {}

		DebugGraphicsComponent(const putils::Point3f & center, float radius, unsigned int color)
			: startPos(center), radius(radius), color(color), debugType(Sphere) {}

		DebugGraphicsComponent(const putils::Rect3f & box, unsigned int color)
			: box(box), color(color), debugType(Box) {}

		string text;
		unsigned int textSize;
		string font;

		putils::Point3f startPos;
		putils::Point3f endPos;
		float radius;
		float thickness;
		unsigned int color;
		putils::Rect3f box;

		enum Type {
			Text,
			Line,
			Sphere,
			Box
		};

		int debugType;

		pmeta_get_class_name(DebugGraphicsComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&DebugGraphicsComponent::text),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::startPos),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::endPos),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::radius),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::thickness),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::color),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::box),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::debugType)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}