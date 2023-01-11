#pragma once

// stl
#include <vector>

// putils
#include "putils/color.hpp"
#include "putils/point.hpp"

namespace kengine::data {
	/*!
	 * putils reflect all
	 * used_types: [
	 * 		refltype::line_element,
	 * 		refltype::sphere_element,
	 * 		refltype::box_element,
	 * 		refltype::element
	 * ]
	 */
	struct debug_graphics {
		//! putils reflect all
		//! class_name: debug_graphics_line
		//! used_types: [putils::point3f]
		struct line_element {
			putils::point3f end{ 0.f, 0.f, 0.f };
			float thickness = 1.f;
		};

		//! putils reflect all
		//! class_name: debug_graphics_sphere
		struct sphere_element {
			float radius = .5f;
		};

		//! putils reflect all
		//! class_name: debug_graphics_box
		//! used_types: [putils::vec3f]
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

		//! putils reflect all
		//! class_name: debug_graphics_element
		//! used_types: [putils::point3f, putils::normalized_color]
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

#include "debug_graphics.reflection.hpp"