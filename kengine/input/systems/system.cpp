#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/input/data/buffer.hpp"
#include "kengine/input/data/handler.hpp"
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::input {
	struct system {
		static constexpr auto log_category = "input";

		entt::registry & r;
		input::buffer * buffer;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			buffer = &e.emplace<input::buffer>();
			e.emplace<main_loop::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			for (const auto & [e, comp] : r.view<handler>().each()) {
				kengine_logf(r, very_verbose, log_category, "Forwarding buffered events to {}", e);

				for (const auto & event : buffer->keys)
					if (comp.on_key != nullptr) {
						kengine_logf(r, very_verbose, log_category, "Forwarding key event {} to {}", event, e);
						comp.on_key({ r, event.window }, event.key, event.pressed);
					}

				if (comp.on_mouse_button != nullptr)
					for (const auto & event : buffer->clicks) {
						kengine_logf(r, very_verbose, log_category, "Forwarding mouse button event {} to {}", event, e);
						comp.on_mouse_button({ r, event.window }, event.button, event.pos, event.pressed);
					}

				if (comp.on_mouse_move != nullptr)
					for (const auto & event : buffer->moves) {
						kengine_logf(r, very_verbose, log_category, "Forwarding mouse move event {} to {}", event, e);
						comp.on_mouse_move({ r, event.window }, event.pos, event.rel);
					}

				if (comp.on_scroll != nullptr)
					for (const auto & event : buffer->scrolls) {
						kengine_logf(r, very_verbose, log_category, "Forwarding scroll event {} to {}", event, e);
						comp.on_scroll({ r, event.window }, event.xoffset, event.yoffset, event.pos);
					}
			}

			kengine_log(r, very_verbose, log_category, "Clearing buffered events");
			buffer->keys.clear();
			buffer->clicks.clear();
			buffer->moves.clear();
			buffer->scrolls.clear();
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}