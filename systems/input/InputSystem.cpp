#include "InputSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

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
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "InputSystem");

			_r = &r;

			const auto e = r.create();
			_buffer = &r.emplace<InputBufferComponent>(e);
			r.emplace<functions::Execute>(e, execute);
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Verbose, "Execute", "InputSystem");

			for (const auto & [e, comp] : _r->view<InputComponent>().each()) {
				for (const auto & e : _buffer->keys)
					if (comp.onKey != nullptr)
						comp.onKey({ *_r, e.window }, e.key, e.pressed);

				if (comp.onMouseButton != nullptr)
					for (const auto & e : _buffer->clicks)
						comp.onMouseButton({ *_r, e.window }, e.button, e.pos, e.pressed);

				if (comp.onMouseMove != nullptr)
					for (const auto & e : _buffer->moves)
						comp.onMouseMove({ *_r, e.window }, e.pos, e.rel);

				if (comp.onScroll != nullptr)
					for (const auto & e : _buffer->scrolls)
						comp.onScroll({ *_r, e.window }, e.xoffset, e.yoffset, e.pos);
			}
			_buffer->keys.clear();
			_buffer->clicks.clear();
			_buffer->moves.clear();
			_buffer->scrolls.clear();
		}

		static inline entt::registry * _r;
		static inline InputBufferComponent * _buffer;
	};

	void InputSystem(entt::registry & r) noexcept {
		InputSystem::init(r);
	}
}