#pragma once

// entt
#include <entt/entity/entity.hpp>

// putils
#include "putils/rect.hpp"

// kengine render
#include "kengine/render/data/on_screen.hpp"

namespace kengine::render {
	//! putils reflect all
	//! parents: [kengine::render::on_screen]
	//! used_types: [putils::rect2f, putils::point2i]
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

#include "viewport.rpp"