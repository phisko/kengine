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
			putils::Point3f end = {};
			float thickness = 1.f;
		};

		struct Sphere {
			float radius = .5f;
		};

		struct Box {
			putils::Vector3f size = { 1.f, 1.f, 1.f };
		};

		enum class ReferenceSpace {
			World,
			Object
		};

		struct Element {
			putils::Point3f pos;
			putils::NormalizedColor color;
			std::variant<Text, Line, Sphere, Box> data;
			ReferenceSpace referenceSpace;

			Element() = default;
			Element(const Element &) = default;
			Element & operator=(const Element &) = default;
			Element(Element &&) = default;
			Element & operator=(Element &&) = default;

			template<typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, Element>()>>
			explicit Element(T && val, const putils::Point3f & pos = { 0.f, 0.f, 0.f }, const putils::NormalizedColor & color = {}, ReferenceSpace referenceSpace = ReferenceSpace::Object)
				: data(FWD(val)), pos(pos), color(color), referenceSpace(referenceSpace)
			{}
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
		putils_reflection_type(refltype::Element),
		putils_reflection_type(refltype::Text),
		putils_reflection_type(refltype::Line),
		putils_reflection_type(refltype::Sphere),
		putils_reflection_type(refltype::Box)
	);
};
#undef refltype

#define refltype kengine::DebugGraphicsComponent::Element
putils_reflection_info{
	putils_reflection_custom_class_name(DebugGraphicsComponentElement);
	putils_reflection_attributes(
		putils_reflection_attribute(pos),
		putils_reflection_attribute(color),
		putils_reflection_attribute(data)
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
