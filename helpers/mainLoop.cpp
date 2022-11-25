#include "mainLoop.hpp"

// stl
#include <chrono>

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "data/TimeModulatorComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/isRunning.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::mainLoop {
	template<typename F>
	static void runFrame(const entt::registry & r, float deltaTime, F && getTimeFactor) noexcept {
		KENGINE_PROFILING_SCOPE;

		deltaTime *= getTimeFactor(r);

		kengine_logf(r, Verbose, "MainLoop", "Calling Execute (dt: %f)", deltaTime);
		for (const auto & [e, func] : r.view<kengine::functions::Execute>().each()) {
			if (!isRunning(r))
				break;
			func(deltaTime);
		}
	}

	template<typename F>
	static void run(entt::registry & r, F && getTimeFactor) noexcept {
		kengine_log(r, Log, "MainLoop", "Starting");

		auto previousTime = std::chrono::system_clock::now();
		while (kengine::isRunning(r)) {
			const auto now = std::chrono::system_clock::now();
			const float deltaTime = std::chrono::duration<float, std::ratio<1>>(now - previousTime).count();
			previousTime = now;

			runFrame(r, deltaTime, getTimeFactor);

			KENGINE_PROFILING_FRAME;
		}

		kengine_log(r, Log, "MainLoop", "Stopping due to no more KeepAlive components remaining");
	}

	void run(entt::registry & r) noexcept {
		run(r, [](auto && ...) noexcept { return 1.f; });
	}

	namespace timeModulated {
		static float getTimeFactor(const entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;

			float ret = 1.f;
			for (const auto & [e, timeModulator] : r.view<TimeModulatorComponent>().each())
				ret *= timeModulator.factor;
			return ret;
		}

		void run(entt::registry & r) noexcept {
			mainLoop::run(r, getTimeFactor);
		}
	}
}