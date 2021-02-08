#pragma once

#include <vector>
#include <variant>
#include "Color.hpp"
#include "Point.hpp"

namespace kengine {
	struct DebugGraphicsComponent {
		struct Text {
			std::string text;
			std::string font;
			float size = 1.f;
		};

		struct Line {
			putils::Point3f end{ 0.f, 0.f, 0.f };
			float thickness = 1.f;
		};

		struct Sphere {
			float radius = .5f;
		};

		struct Box {
			putils::Vector3f size = { 1.f, 1.f, 1.f };
		};

		enum class Type {
			Text,
			Line,
			Sphere,
			Box
		};

		enum class ReferenceSpace {
			World,
			Object
		};

		struct Element {
			putils::Point3f pos{ 0.f, 0.f, 0.f };
			putils::NormalizedColor color;

			ReferenceSpace referenceSpace = ReferenceSpace::Object;

			Text text;
			Line line;
			Sphere sphere;
			Box box;

			Type type;
		};

		std::vector<Element> elements;
	};
}

#define refltype kengine::DebugGraphicsComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(elements)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::Text),
		putils_reflection_type(refltype::Line),
		putils_reflection_type(refltype::Sphere),
		putils_reflection_type(refltype::Box),
		putils_reflection_type(refltype::Element)
	);
};
#undef refltype

#define refltype kengine::DebugGraphicsComponent::Element
putils_reflection_info{
	putils_reflection_custom_class_name(DebugGraphicsComponentElement);
	putils_reflection_attributes(
		putils_reflection_attribute(pos),
		putils_reflection_attribute(color),
		putils_reflection_attribute(text),
		putils_reflection_attribute(line),
		putils_reflection_attribute(sphere),
		putils_reflection_attribute(box),
		putils_reflection_attribute(type),
		putils_reflection_attribute(referenceSpace)
	);
};
#undef refltype

#define refltype kengine::DebugGraphicsComponent::Box
putils_reflection_info{
	putils_reflection_custom_class_name(DebugGraphicsComponentBox);
	putils_reflection_attributes(
		putils_reflection_attribute(size)
	);
};
#undef refltype

#define refltype kengine::DebugGraphicsComponent::Sphere
putils_reflection_info{
	putils_reflection_custom_class_name(DebugGraphicsComponentSphere);
	putils_reflection_attributes(
		putils_reflection_attribute(radius)
	);
};
#undef refltype

#define refltype kengine::DebugGraphicsComponent::Line
putils_reflection_info{
	putils_reflection_custom_class_name(DebugGraphicsComponentLine);
	putils_reflection_attributes(
		putils_reflection_attribute(end),
		putils_reflection_attribute(thickness)
	);
};
#undef refltype

#define refltype kengine::DebugGraphicsComponent::Text
putils_reflection_info{
	putils_reflection_custom_class_name(DebugGraphicsComponentText);
	putils_reflection_attributes(
		putils_reflection_attribute(text),
		putils_reflection_attribute(font),
		putils_reflection_attribute(size)
	);
};
#undef refltype
