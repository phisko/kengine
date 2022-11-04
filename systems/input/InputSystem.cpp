#include "InputSystem.hpp"
#include "kengine.hpp"

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
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "InputSystem");

			_buffer = &e.attach<InputBufferComponent>();
			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Verbose, "Execute", "InputSystem");

			for (const auto & [e, comp] : entities.with<InputComponent>()) {
				for (const auto & e : _buffer->keys)
					if (comp.onKey != nullptr)
						comp.onKey(e.window, e.key, e.pressed);

				if (comp.onMouseButton != nullptr)
					for (const auto & e : _buffer->clicks)
						comp.onMouseButton(e.window, e.button, e.pos, e.pressed);

				if (comp.onMouseMove != nullptr)
					for (const auto & e : _buffer->moves)
						comp.onMouseMove(e.window, e.pos, e.rel);

				if (comp.onScroll != nullptr)
					for (const auto & e : _buffer->scrolls)
						comp.onScroll(e.window, e.xoffset, e.yoffset, e.pos);
			}
			_buffer->keys.clear();
			_buffer->clicks.clear();
			_buffer->moves.clear();
			_buffer->scrolls.clear();
		}

		static inline InputBufferComponent * _buffer;
	};

	EntityCreator * InputSystem() noexcept {
		return InputSystem::init;
	}
}