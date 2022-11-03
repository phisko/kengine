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

namespace kengine::input {
	struct impl {
		static inline InputBufferComponent * buffer;

		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "InputSystem");

			buffer = &e.attach<InputBufferComponent>();
			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Verbose, "Execute", "InputSystem");

			for (const auto & [e, comp] : entities.with<InputComponent>()) {
				for (const auto & e : buffer->keys)
					if (comp.onKey != nullptr)
						comp.onKey(e.window, e.key, e.pressed);

				if (comp.onMouseButton != nullptr)
					for (const auto & e : buffer->clicks)
						comp.onMouseButton(e.window, e.button, e.pos, e.pressed);

				if (comp.onMouseMove != nullptr)
					for (const auto & e : buffer->moves)
						comp.onMouseMove(e.window, e.pos, e.rel);

				if (comp.onScroll != nullptr)
					for (const auto & e : buffer->scrolls)
						comp.onScroll(e.window, e.xoffset, e.yoffset, e.pos);
			}
			buffer->keys.clear();
			buffer->clicks.clear();
			buffer->moves.clear();
			buffer->scrolls.clear();
		}
	};
}

namespace kengine {
	EntityCreator * InputSystem() noexcept {
		KENGINE_PROFILING_SCOPE;
		return [](Entity & e) noexcept {
			input::impl::init(e);
		};
	}
}