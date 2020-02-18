#include <chrono>

#include "MainLoop.hpp"
#include "EntityManager.hpp"
#include "functions/Execute.hpp"

namespace kengine::MainLoop {
	void run(EntityManager & em) {
		auto start = std::chrono::system_clock::now();
		auto end = std::chrono::system_clock::now();
		while (em.running) {
			const float deltaTime = std::chrono::duration<float, std::ratio<1>>(end - start).count();

			start = std::chrono::system_clock::now();
			for (const auto & [e, func] : em.getEntities<functions::Execute>())
				func(deltaTime);
			end = std::chrono::system_clock::now();
		}
	}
}