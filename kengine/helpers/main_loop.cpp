#include "main_loop.hpp"

// stl
#include <chrono>

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "kengine/data/time_modulator.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/is_running.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::main_loop {
	template<typename F>
	static void run_frame(const entt::registry & r, float delta_time, F && get_time_factor) noexcept {
		KENGINE_PROFILING_SCOPE;

		delta_time *= get_time_factor(r);

		kengine_logf(r, verbose, "MainLoop", "Calling execute (dt: %f)", delta_time);
		for (const auto & [e, func] : r.view<kengine::functions::execute>().each()) {
			if (!is_running(r))
				break;
			func(delta_time);
		}
	}

	template<typename F>
	static void run(entt::registry & r, F && get_time_factor) noexcept {
		kengine_log(r, log, "MainLoop", "Starting");

		auto previous_time = std::chrono::system_clock::now();
		while (kengine::is_running(r)) {
			const auto now = std::chrono::system_clock::now();
			const float delta_time = std::chrono::duration<float>(now - previous_time).count();
			previous_time = now;

			run_frame(r, delta_time, get_time_factor);

			KENGINE_PROFILING_FRAME;
		}

		kengine_log(r, log, "MainLoop", "Stopping due to no more data::keep_alive components remaining");
	}

	void run(entt::registry & r) noexcept {
		run(r, [](auto &&...) noexcept { return 1.f; });
	}

	namespace time_modulated {
		static float get_time_factor(const entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;

			float ret = 1.f;
			for (const auto & [e, time_modulator] : r.view<data::time_modulator>().each())
				ret *= time_modulator.factor;
			return ret;
		}

		void run(entt::registry & r) noexcept {
			main_loop::run(r, get_time_factor);
		}
	}
}