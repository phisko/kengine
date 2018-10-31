#pragma once

#include "EntityManager.hpp"
#include "SerializableComponent.hpp"

namespace kengine {
	class DebugGraphicsComponent : public kengine::SerializableComponent<DebugGraphicsComponent> {
	public:
		DebugGraphicsComponent() = default;

		DebugGraphicsComponent(const std::string & text, unsigned int textSize, const std::string & font, const putils::Point3f & startPos, unsigned int color)
			: text(text), textSize(textSize), font(font), startPos(startPos), color(color), debugType(Text) {}

		DebugGraphicsComponent(const putils::Point3f & startPos, const putils::Point3f & endPos, float thickness, unsigned int color)
			: startPos(startPos), endPos(endPos), thickness(thickness), color(color), debugType(Line) {}

		DebugGraphicsComponent(const putils::Point3f & center, float radius, unsigned int color)
			: startPos(center), radius(radius), color(color), debugType(Sphere) {}

		DebugGraphicsComponent(const putils::Rect3f & box, unsigned int color)
			: box(box), color(color), debugType(Box) {}

		const std::string type = pmeta_nameof(DebugGraphicsComponent);

		std::string text;
		unsigned int textSize;
		std::string font;

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
			pmeta_reflectible_attribute(&DebugGraphicsComponent::type),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::text),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::startPos),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::endPos),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::radius),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::thickness),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::color),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::box),
			pmeta_reflectible_attribute(&DebugGraphicsComponent::debugType)
		);
	};
}