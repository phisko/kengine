#pragma once

// stl
#include <vector>

// putils
#include "putils/color.hpp"
#include "putils/point.hpp"

namespace kengine::data {
	struct debug_graphics {
		struct line_element {
			putils::point3f end{ 0.f, 0.f, 0.f };
			float thickness = 1.f;
		};

		struct sphere_element {
			float radius = .5f;
		};

		struct box_element {
			putils::vec3f size = { 1.f, 1.f, 1.f };
		};

		enum class element_type {
			line,
			sphere,
			box
		};

		enum class reference_space {
			world,
			object
		};

		struct element {
			putils::point3f pos{ 0.f, 0.f, 0.f };
			putils::normalized_color color;

			reference_space relative_to = reference_space::object;

			line_element line;
			sphere_element sphere;
			box_element box;

			element_type type;
		};

		std::vector<element> elements;
	};
}

#define refltype kengine::data::debug_graphics
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(elements)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::line_element),
		putils_reflection_type(refltype::sphere_element),
		putils_reflection_type(refltype::box_element),
		putils_reflection_type(refltype::element)
	);
};
#undef refltype

#define refltype kengine::data::debug_graphics::element
putils_reflection_info{
	putils_reflection_custom_class_name(debug_graphics_element);
	putils_reflection_attributes(
		putils_reflection_attribute(pos),
		putils_reflection_attribute(color),
		putils_reflection_attribute(line),
		putils_reflection_attribute(sphere),
		putils_reflection_attribute(box),
		putils_reflection_attribute(type),
		putils_reflection_attribute(relative_to)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::point3f),
		putils_reflection_type(putils::normalized_color)
	);
};
#undef refltype

#define refltype kengine::data::debug_graphics::box_element
putils_reflection_info{
	putils_reflection_custom_class_name(debug_graphics_box);
	putils_reflection_attributes(
		putils_reflection_attribute(size)
	);
};
#undef refltype

#define refltype kengine::data::debug_graphics::sphere_element
putils_reflection_info{
	putils_reflection_custom_class_name(debug_graphics_sphere);
	putils_reflection_attributes(
		putils_reflection_attribute(radius)
	);
};
#undef refltype

#define refltype kengine::data::debug_graphics::line_element
putils_reflection_info{
	putils_reflection_custom_class_name(debug_graphics_line);
	putils_reflection_attributes(
		putils_reflection_attribute(end),
		putils_reflection_attribute(thickness)
	);
};
#undef refltype