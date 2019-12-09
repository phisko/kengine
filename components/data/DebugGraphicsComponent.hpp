#pragma once

#ifndef KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH
# define KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH 64
#endif

#include "string.hpp"
#include "Color.hpp"
#include "Point.hpp"

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
		DebugGraphicsComponent(Type type, const putils::Rect3f & offset = { {}, { 1.f, 1.f, 1.f } }, const putils::NormalizedColor & color = {})
			: offset(offset), color(color), debugType(type) {}

		string text; 
		string font;

		putils::NormalizedColor color;
		putils::Rect3f offset{ {}, { 1.f, 1.f, 1.f } };
		putils::Point3f lineEnd;
		// text size: offset.size.x
		// circle radius: offset.size.x
		// line thickness: offset.size.y

		Type debugType = Type::Box;

		putils_reflection_class_name(DebugGraphicsComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&DebugGraphicsComponent::text),
			putils_reflection_attribute(&DebugGraphicsComponent::font),
			putils_reflection_attribute(&DebugGraphicsComponent::color),
			putils_reflection_attribute(&DebugGraphicsComponent::offset),
			putils_reflection_attribute(&DebugGraphicsComponent::debugType)
		);
	};
}