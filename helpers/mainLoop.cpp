#include <chrono>

#include "MainLoop.hpp"
#include "EntityManager.hpp"

#include "data/TimeModulatorComponent.hpp"
#include "functions/Execute.hpp"

template<typename F>
static void run(kengine::EntityManager & em, F && getTimeFactor) {
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	while (em.running) {
		const float deltaTime = getTimeFactor(em) * std::chrono::duration<float, std::ratio<1>>(end - start).count();

		start = std::chrono::system_clock::now();
		for (const auto & [e, func] : em.getEntities<kengine::functions::Execute>())
			func(deltaTime);
		end = std::chrono::system_clock::now();
	}
}

namespace kengine {
	namespace mainLoop {
		void run(EntityManager & em) {
			::run(em, [](EntityManager & em) { return 1.f; });
		}

		namespace timeModulated {
			void run(EntityManager & em) {
				::run(em, [](EntityManager & em) {
					float ret = 1.f;
					for (const auto & [e, timeModulator] : em.getEntities<TimeModulatorComponent>())
						ret *= timeModulator.factor;
					return ret;
				});
			}
		}
	}
}