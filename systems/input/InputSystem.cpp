#include "InputSystem.hpp"
#include "kengine.hpp"
#include "data/InputComponent.hpp"
#include "data/InputBufferComponent.hpp"
#include "functions/Execute.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::input {
	struct impl {
		static inline InputBufferComponent * buffer;

		static void init(Entity & e) noexcept {
			kengine_log(Log, "Init", "InputSystem");
			buffer = &e.attach<InputBufferComponent>();
			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
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
		return [](Entity & e) noexcept {
			input::impl::init(e);
		};
	}
}