#include "mainLoop.hpp"
#include "kengine.hpp"

// stl
#include <chrono>

// kengine data
#include "data/TimeModulatorComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"

template<typename F>
static void run(F && getTimeFactor) noexcept {
	kengine_log(Log, "MainLoop", "Starting");

	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();
	while (kengine::isRunning()) {
		const float deltaTime = getTimeFactor() * std::chrono::duration<float, std::ratio<1>>(end - start).count();

		start = std::chrono::system_clock::now();
		kengine_logf(Verbose, "MainLoop", "Calling Execute (dt: %f)", deltaTime);
		for (const auto & [e, func] : kengine::entities.with<kengine::functions::Execute>())
			func(deltaTime);
		kengine_log(Verbose, "MainLoop", "Cleaning up archetypes");
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