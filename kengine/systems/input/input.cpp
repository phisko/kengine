#include "input.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

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
			kengine_log(r, log, "Init", "systems/input");

			buffer = &e.emplace<data::input_buffer>();
			e.emplace<functions::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "execute", "input");

			for (const auto & [e, comp] : r.view<data::input>().each()) {
				for (const auto & e : buffer->keys)
					if (comp.on_key != nullptr)
						comp.on_key({ r, e.window }, e.key, e.pressed);

				if (comp.on_mouse_button != nullptr)
					for (const auto & e : buffer->clicks)
						comp.on_mouse_button({ r, e.window }, e.button, e.pos, e.pressed);

				if (comp.on_mouse_move != nullptr)
					for (const auto & e : buffer->moves)
						comp.on_mouse_move({ r, e.window }, e.pos, e.rel);

				if (comp.on_scroll != nullptr)
					for (const auto & e : buffer->scrolls)
						comp.on_scroll({ r, e.window }, e.xoffset, e.yoffset, e.pos);
			}
			buffer->keys.clear();
			buffer->clicks.clear();
			buffer->moves.clear();
			buffer->scrolls.clear();
		}
	};

	void add_input(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<input>(e);
	}
}