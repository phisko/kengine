#include "on_click.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine input
#include "kengine/input/data/input.hpp"

// kengine render
#include "kengine/render/functions/get_entity_in_pixel.hpp"

// kengine render/on_click
#include "kengine/render/on_click/functions/on_click.hpp"

namespace kengine::systems {
	struct on_click {
		on_click(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*e.registry(), log, "on_click", "Initializing");

			e.emplace<data::input>().on_mouse_button = on_mouse_button;
		}

		static void on_mouse_button(entt::handle window, int button, const putils::point2f & coords, bool pressed) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!pressed)
				return;

			const auto & r = *window.registry();
			kengine_logf(r, verbose, "on_click", "Click in { %f, %f }", coords.x, coords.y);

			for (const auto & [_, get_entity] : r.view<functions::get_entity_in_pixel>().each()) {
				const auto e = get_entity(window, coords);
				if (e == entt::null) {
					kengine_log(r, verbose, "on_click", "No entity found in pixel");
					continue;
				}

				const auto on_click = r.try_get<functions::on_click>(e);
				if (on_click) {
					kengine_logf(r, log, "on_click", "Calling on_click on [%u]", e);
					on_click->call(button);
				}
				else
					kengine_logf(r, verbose, "on_click", "Clicked [%u], did not have on_click", e);
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(on_click)
}
