#include "InputSystem.hpp"
#include "EntityManager.hpp"
#include "data/InputComponent.hpp"
#include "data/InputBufferComponent.hpp"
#include "functions/Execute.hpp"

namespace kengine::input {
	struct impl {
		static inline InputBufferComponent * buffer;
		static inline EntityManager * em;

		static void init(Entity & e) {
			buffer = &e.attach<InputBufferComponent>();
			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) {
			for (const auto & [e, comp] : em->getEntities<InputComponent>()) {
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
	EntityCreatorFunctor<64> InputSystem(EntityManager & em) {
		input::impl::em = &em;
		return [&](Entity & e) {
			input::impl::init(e);
		};
	}
}