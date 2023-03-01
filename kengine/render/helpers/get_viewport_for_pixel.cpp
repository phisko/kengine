#include "get_viewport_for_pixel.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine render
#include "kengine/render/data/viewport.hpp"
#include "kengine/render/data/window.hpp"
#include "kengine/render/helpers/convert_to_screen_percentage.hpp"

namespace kengine::render {
	viewport_info get_viewport_for_pixel(entt::handle window_entity, const putils::point2ui & pixel) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*window_entity.registry(), very_verbose, "camera_helper", "Getting viewport for [%u]'s pixel { %zu, %zu }", window_entity.entity(), pixel.x, pixel.y);

		viewport_info ret;

		const auto & window = window_entity.get<render::window>();

		float highest_z = -std::numeric_limits<float>::max();
		for (const auto & [e, viewport] : window_entity.registry()->view<render::viewport>().each()) {
			if (viewport.window != window_entity) {
				kengine_logf(*window_entity.registry(), very_verbose, "camera_helper", "Dismissing viewport [%u] because its window ([%u]) does not match", e, viewport.window);
				continue;
			}

			if (highest_z != -std::numeric_limits<float>::max() && highest_z >= viewport.z_order) {
				kengine_logf(*window_entity.registry(), very_verbose, "camera_helper", "Dismissing viewport [%u] because its z-order (%f) is below previously found [%u]'s (%f)", e, viewport.z_order, ret.camera, highest_z);
				continue;
			}

			const auto box = convert_to_screen_percentage(viewport.bounding_box, window.size, viewport);

			const auto start_x = box.position.x * window.size.x;
			const auto start_y = box.position.y * window.size.y;
			const auto size_x = box.size.x * window.size.x;
			const auto size_y = box.size.y * window.size.y;

			if (pixel.x < start_x || pixel.y < start_y ||
				pixel.x >= start_x + size_x ||
				pixel.y >= start_y + size_y) {
				kengine_logf(*window_entity.registry(), very_verbose, "camera_helper", "Dismissing viewport [%u] because the pixel isn't in its bounds", e);
				continue;
			}

			highest_z = viewport.z_order;

			ret.camera = e;
			ret.pixel = putils::point2f(pixel) / window.size;
			ret.viewport_percent = (ret.pixel - putils::point2f(box.position)) / putils::point2f(box.size);
		}

		kengine_logf(*window_entity.registry(), very_verbose, "camera_helper", "Found camera [%u], pixel { %zu, %zu }, viewport percentage { %f, %f }", ret.camera, ret.pixel.x, ret.pixel.y, ret.viewport_percent.x, ret.viewport_percent.y);
		return ret;
	}
}