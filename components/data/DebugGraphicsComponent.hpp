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

			putils_reflection_class_name(DebugGraphicsComponentText);
			putils_reflection_attributes(
				putils_reflection_attribute(&Text::text),
				putils_reflection_attribute(&Text::font),
				putils_reflection_attribute(&Text::size)
			);
		};

		struct Line {
			putils::Point3f end = {};
			float thickness = 1.f;

			putils_reflection_class_name(DebugGraphicsComponentLine);
			putils_reflection_attributes(
				putils_reflection_attribute(&Line::end),
				putils_reflection_attribute(&Line::thickness)
			);
		};

		struct Sphere {
			float radius = .5f;

			putils_reflection_class_name(DebugGraphicsComponentSphere);
			putils_reflection_attributes(
				putils_reflection_attribute(&Sphere::radius)
			);
		};

		struct Box {
			putils::Vector3f size = { 1.f, 1.f, 1.f };

			putils_reflection_class_name(DebugGraphicsComponentBox);
			putils_reflection_attributes(
				putils_reflection_attribute(&Box::size)
			);
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
			explicit Element(T && val, const putils::Point3f & pos = {}, const putils::NormalizedColor & color = {}, ReferenceSpace referenceSpace = ReferenceSpace::Object)
				: data(FWD(val)), pos(pos), color(color), referenceSpace(referenceSpace)
			{}

			putils_reflection_class_name(DebugGraphicsComponentElement);
			putils_reflection_attributes(
				putils_reflection_attribute(&Element::pos),
				putils_reflection_attribute(&Element::color),
				putils_reflection_attribute(&Element::data)
			);
		};

		std::vector<Element> elements;

		putils_reflection_class_name(DebugGraphicsComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&DebugGraphicsComponent::elements)
		);
		putils_reflection_used_types(
			putils_reflection_type(Element),
			putils_reflection_type(Text),
			putils_reflection_type(Line),
			putils_reflection_type(Sphere),
			putils_reflection_type(Box)
		);
	};
}