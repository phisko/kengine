#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/rect.hpp"

// kengine data
#include "kengine/data/on_screen.hpp"

namespace kengine::data {
	struct viewport : on_screen {
		using render_texture = std::intptr_t;
		static constexpr auto INVALID_RENDER_TEXTURE = -1;

		putils::rect2f bounding_box = { { 0.f, 0.f }, { 1.f, 1.f } };
		putils::point2i resolution = { 1280, 720 };
		float z_order = 1.f;
		entt::entity window = entt::null;
		render_texture texture = INVALID_RENDER_TEXTURE;

		viewport() noexcept {
			coordinates = coordinate_type::screen_percentage;
		}
	};
}

#define refltype kengine::data::viewport
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(window),
		putils_reflection_attribute(bounding_box),
		putils_reflection_attribute(resolution),
		putils_reflection_attribute(z_order)
	);
	putils_reflection_parents(
		putils_reflection_type(kengine::data::on_screen)
	);
};
#undef refltype