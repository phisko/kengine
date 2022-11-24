#include "InputSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "forward_to.hpp"

// kengine data
#include "data/InputComponent.hpp"
#include "data/InputBufferComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct InputSystem {
		entt::registry & r;
		InputBufferComponent * buffer;

		InputSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "InputSystem");

			buffer = &e.emplace<InputBufferComponent>();
			e.emplace<functions::Execute>(putils_forward_to_this(execute));
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute", "InputSystem");

			for (const auto & [e, comp] : r.view<InputComponent>().each()) {
				for (const auto & e : buffer->keys)
					if (comp.onKey != nullptr)
						comp.onKey({ r, e.window }, e.key, e.pressed);

				if (comp.onMouseButton != nullptr)
					for (const auto & e : buffer->clicks)
						comp.onMouseButton({ r, e.window }, e.button, e.pos, e.pressed);

				if (comp.onMouseMove != nullptr)
					for (const auto & e : buffer->moves)
						comp.onMouseMove({ r, e.window }, e.pos, e.rel);

				if (comp.onScroll != nullptr)
					for (const auto & e : buffer->scrolls)
						comp.onScroll({ r, e.window }, e.xoffset, e.yoffset, e.pos);
			}
			buffer->keys.clear();
			buffer->clicks.clear();
			buffer->moves.clear();
			buffer->scrolls.clear();
		}
	};

	void addInputSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<InputSystem>(e);
	}
}