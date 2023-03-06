#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/input/data/handler.hpp"
#include "kengine/render/functions/get_entity_in_pixel.hpp"
#include "kengine/render/on_click/functions/on_click.hpp"

namespace kengine::render::on_click {
	static constexpr auto log_category = "render_on_click";

	struct system {
		system(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*e.registry(), log, log_category, "Initializing");

			e.emplace<input::handler>().on_mouse_button = on_mouse_button;
		}

		static void on_mouse_button(entt::handle window, int button, const putils::point2f & coords, bool pressed) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!pressed)
				return;

			const auto & r = *window.registry();
			kengine_logf(r, verbose, log_category, "Click in {}", coords);

			for (const auto & [_, get_entity] : r.view<get_entity_in_pixel>().each()) {
				const auto e = get_entity(window, coords);
				if (e == entt::null) {
					kengine_log(r, verbose, log_category, "No entity found in pixel");
					continue;
				}

				const auto on_click = r.try_get<render::on_click::on_click>(e);
				if (on_click) {
					kengine_logf(r, log, log_category, "Calling on_click on {}", e);
					on_click->call(button);
				}
				else
					kengine_logf(r, verbose, log_category, "Clicked {}, did not have on_click", e);
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}
