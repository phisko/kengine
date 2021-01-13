#include <chrono>

#include "MainLoop.hpp"
#include "kengine.hpp"

#include "data/TimeModulatorComponent.hpp"
#include "functions/Execute.hpp"

template<typename F>
static void run(F && getTimeFactor) noexcept {
	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	while (kengine::isRunning()) {
		const float deltaTime = getTimeFactor() * std::chrono::duration<float, std::ratio<1>>(end - start).count();

		start = std::chrono::system_clock::now();
		for (const auto & [e, func] : kengine::entities.with<kengine::functions::Execute>())
			func(deltaTime);
		kengine::cleanupArchetypes();
		end = std::chrono::system_clock::now();
	}
}

namespace kengine {
	namespace mainLoop {
		void run() noexcept {
			::run([]() noexcept { return 1.f; });
		}

		namespace timeModulated {
			void run() noexcept {
				::run([]() noexcept {
					float ret = 1.f;
					for (const auto & [e, timeModulator] : entities.with<TimeModulatorComponent>())
						ret *= timeModulator.factor;
					return ret;
				});
			}
		}
	}
}