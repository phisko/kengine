#pragma once

// putils
#include "putils/rect.hpp"

// kengine
#include "kengine/render/data/on_screen.hpp"

namespace kengine::render {
	KENGINE_RENDER_EXPORT putils::rect3f convert_to_screen_percentage(const putils::rect3f & rect, const putils::point2f & screen_size, const on_screen & comp) noexcept;
	KENGINE_RENDER_EXPORT putils::rect2f convert_to_screen_percentage(const putils::rect2f & rect, const putils::point2f & screen_size, const on_screen & comp) noexcept;
}