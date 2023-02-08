#include "input.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"
#include "putils/reflection_helpers/json_helper.hpp"

// kengine data
#include "kengine/data/input.hpp"
#include "kengine/data/input_buffer.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct input {
		entt::registry & r;
		data::input_buffer * buffer;

		input(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "input", "Initializing");

			buffer = &e.emplace<data::input_buffer>();
			e.emplace<functions::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "input", "Executing");

			for (const auto & [e, comp] : r.view<data::input>().each()) {
				kengine_logf(r, very_verbose, "input", "Forwarding buffered events to [%u]", e);

				for (const auto & event : buffer->keys)
					if (comp.on_key != nullptr) {
						kengine_logf(r, very_verbose, "input", "Forwarding key event %s to [%u]", putils::reflection::to_json(event).dump().c_str(), e);
						comp.on_key({ r, event.window }, event.key, event.pressed);
					}

				if (comp.on_mouse_button != nullptr)
					for (const auto & event : buffer->clicks) {
						kengine_logf(r, very_verbose, "input", "Forwarding mouse button event %s to [%u]", putils::reflection::to_json(event).dump().c_str(), e);
						comp.on_mouse_button({ r, event.window }, event.button, event.pos, event.pressed);
					}

				if (comp.on_mouse_move != nullptr)
					for (const auto & event : buffer->moves) {
						kengine_logf(r, very_verbose, "input", "Forwarding mouse move event %s to [%u]", putils::reflection::to_json(event).dump().c_str(), e);
						comp.on_mouse_move({ r, event.window }, event.pos, event.rel);
					}

				if (comp.on_scroll != nullptr)
					for (const auto & event : buffer->scrolls) {
						kengine_logf(r, very_verbose, "input", "Forwarding scroll event %s to [%u]", putils::reflection::to_json(event).dump().c_str(), e);
						comp.on_scroll({ r, event.window }, event.xoffset, event.yoffset, event.pos);
					}
			}

			kengine_log(r, very_verbose, "input", "Clearing buffered events");
			buffer->keys.clear();
			buffer->clicks.clear();
			buffer->moves.clear();
			buffer->scrolls.clear();
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(input)
}