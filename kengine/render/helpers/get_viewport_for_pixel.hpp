#pragma once

// entt
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

// putils
#include "putils/rect.hpp"

namespace kengine::render {
	struct viewport_info {
		entt::entity camera = entt::null;
		putils::point2f pixel = { -1.f, -1.f };
		putils::point2f viewport_percent = { -1.f, -1.f }; // [0,1]
	};
	KENGINE_RENDER_EXPORT viewport_info get_viewport_for_pixel(entt::handle window_entity, const putils::point2ui & pixel) noexcept;
}