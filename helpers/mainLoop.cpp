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
#include "helpers/profilingHelper.hpp"

namespace kengine {
	namespace mainLoop {
		template<typename F>
		static void runFrame(float deltaTime, F && getTimeFactor) noexcept {
			KENGINE_PROFILING_SCOPE;

			deltaTime *= getTimeFactor();

			kengine_logf(Verbose, "MainLoop", "Calling Execute (dt: %f)", deltaTime);
			for (const auto & [e, func]: kengine::entities.with<kengine::functions::Execute>())
				func(deltaTime);

			kengine_log(Verbose, "MainLoop", "Cleaning up archetypes");
			kengine::cleanupArchetypes();
		}

		template<typename F>
		static void run(F && getTimeFactor) noexcept {
			kengine_log(Log, "MainLoop", "Starting");

			auto previousTime = std::chrono::system_clock::now();
			while (kengine::isRunning()) {
				const auto now = std::chrono::system_clock::now();
				const float deltaTime = std::chrono::duration<float, std::ratio<1>>(now - previousTime).count();
				previousTime = now;

				runFrame(deltaTime, FWD(getTimeFactor));

				KENGINE_PROFILING_FRAME;
			}
		}

		void run() noexcept {
			run([]() noexcept { return 1.f; });
		}

		namespace timeModulated {
			static float getTimeFactor() noexcept {
				KENGINE_PROFILING_SCOPE;

				float ret = 1.f;
				for (const auto & [e, timeModulator] : entities.with<TimeModulatorComponent>())
					ret *= timeModulator.factor;
				return ret;
			}

			void run() noexcept {
				mainLoop::run(getTimeFactor);
			}
		}
	}
}