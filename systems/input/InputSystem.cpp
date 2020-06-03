#include "InputSystem.hpp"
#include "EntityManager.hpp"
#include "data/InputComponent.hpp"
#include "data/InputBufferComponent.hpp"
#include "functions/Execute.hpp"

namespace kengine {
	static InputBufferComponent * g_buffer;

#pragma region declarations
	static void execute(EntityManager & em);
#pragma endregion
	EntityCreatorFunctor<64> InputSystem(EntityManager & em) {
		return [&](Entity & e) {
			e += functions::Execute{ [&](float deltaTime) { execute(em); } };
			g_buffer = &e.attach<InputBufferComponent>();
		};
	}

	static void execute(EntityManager & em) {
		for (const auto &[e, comp] : em.getEntities<InputComponent>()) {
			for (const auto & e : g_buffer->keys)
				if (comp.onKey != nullptr)
					comp.onKey(e.window, e.key, e.pressed);

			if (comp.onMouseButton != nullptr)
				for (const auto & e : g_buffer->clicks)
					comp.onMouseButton(e.window, e.button, e.pos, e.pressed);

			if (comp.onMouseMove != nullptr)
				for (const auto & e : g_buffer->moves)
					comp.onMouseMove(e.window, e.pos, e.rel);

			if (comp.onScroll != nullptr)
				for (const auto & e : g_buffer->scrolls)
					comp.onScroll(e.window, e.xoffset, e.yoffset, e.pos);
		}
		g_buffer->keys.clear();
		g_buffer->clicks.clear();
		g_buffer->moves.clear();
		g_buffer->scrolls.clear();
	}
}