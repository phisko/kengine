#pragma once

#include "EntityManager.hpp"
#include "SerializableComponent.hpp"

namespace kengine {
	class DebugGraphicsComponent : public kengine::SerializableComponent<DebugGraphicsComponent> {
	public:
		DebugGraphicsComponent() = default;

		DebugGraphicsComponent(const std::string & text, unsigned int textSize, const std::string & font, const putils::Point3d & startPos, unsigned int color)
			: text(text), textSize(textSize), font(font), startPos(startPos), color(color), debugType(Text) {}

		DebugGraphicsComponent(const putils::Point3d & startPos, const putils::Point3d & endPos, double thickness, unsigned int color)
			: startPos(startPos), endPos(endPos), thickness(thickness), color(color), debugType(Line) {}

		DebugGraphicsComponent(const putils::Point3d & center, double radius, unsigned int color)
			: startPos(center), radius(radius), color(color), debugType(Sphere) {}

		const std::string type = pmeta_nameof(DebugGraphicsComponent);

		std::string text;
		unsigned int textSize;
		std::string font;

		putils::Point3d startPos;
		putils::Point3d endPos;
		double radius;
		double thickness;
		unsigned int color;

		enum Type {
			Text,
			Line,
			Sphere
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
			pmeta_reflectible_attribute(&DebugGraphicsComponent::debugType)
		);
	};
}