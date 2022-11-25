#pragma once

// entt
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

// putils
#include "putils/rect.hpp"

// kengine data
#include "kengine/data/camera.hpp"
#include "kengine/data/on_screen.hpp"

// kengine functions
#include "kengine/functions/appears_in_viewport.hpp"

namespace kengine::camera_helper {
	struct viewport_info {
		entt::entity camera = entt::null;
		putils::point2f pixel = { -1.f, -1.f };
		putils::point2f viewport_percent = { -1.f, -1.f }; // [0,1]
	};
	KENGINE_CORE_EXPORT viewport_info get_viewport_for_pixel(entt::handle window_entity, const putils::point2ui & pixel) noexcept;

	KENGINE_CORE_EXPORT bool entity_appears_in_viewport(const entt::registry & r, entt::entity entity, entt::entity viewport_entity) noexcept;
    KENGINE_CORE_EXPORT putils::rect3f convert_to_screen_percentage(const putils::rect3f & rect, const putils::point2f & screen_size, const data::on_screen & comp) noexcept;
    KENGINE_CORE_EXPORT putils::rect2f convert_to_screen_percentage(const putils::rect2f & rect, const putils::point2f & screen_size, const data::on_screen & comp) noexcept;

	struct facings {
		putils::vec3f front;
		putils::vec3f right;
		putils::vec3f up;
	};

	KENGINE_CORE_EXPORT facings get_facings(const data::camera & camera) noexcept;
}