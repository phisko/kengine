#include "main_loop.hpp"

// stl
#include <chrono>

// entt
#include <entt/entity/registry.hpp>

// meta
#include "putils/meta/concepts/invocable.hpp"

// kengine core
#include "kengine/core/helpers/log_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine main_loop
#include "kengine/main_loop/data/time_modulator.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/main_loop/helpers/is_running.hpp"

namespace kengine::main_loop {
	template<typename T>
	concept time_factor_callback = putils::invocable<T, float(const entt::registry &)>;

	template<time_factor_callback F>
	static void run_frame(const entt::registry & r, float delta_time, F && get_time_factor) noexcept {
		KENGINE_PROFILING_SCOPE;

		delta_time *= get_time_factor(r);

		kengine_logf(r, very_verbose, "main_loop", "Calling execute (dt: %f)", delta_time);
		for (const auto & [e, func] : r.view<kengine::functions::execute>().each()) {
			if (!is_running(r))
				break;
			kengine_logf(r, very_verbose, "main_loop", "Calling execute on [%u]", e);
			func(delta_time);
		}
	}

	template<time_factor_callback F>
	static void run(entt::registry & r, F && get_time_factor) noexcept {
		kengine_log(r, log, "main_loop", "Starting");

		auto previous_time = std::chrono::system_clock::now();
		while (kengine::is_running(r)) {
			const auto now = std::chrono::system_clock::now();
			const float delta_time = std::chrono::duration<float>(now - previous_time).count();
			previous_time = now;

			run_frame(r, delta_time, get_time_factor);

			KENGINE_PROFILING_FRAME;
		}

		kengine_log(r, log, "main_loop", "Stopping due to no more data::keep_alive components remaining");
	}

	void run(entt::registry & r) noexcept {
		run(r, [](auto &&...) noexcept { return 1.f; });
	}

	namespace time_modulated {
		static float get_time_factor(const entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "main_loop", "Getting time factor");

			float ret = 1.f;
			for (const auto & [e, time_modulator] : r.view<data::time_modulator>().each()) {
				kengine_logf(r, very_verbose, "main_loop", "Found time modulator [%u] (%f)", e, time_modulator.factor);
				ret *= time_modulator.factor;
			}

			kengine_logf(r, very_verbose, "main_loop", "Final time modulator: %f", ret);
			return ret;
		}

		void run(entt::registry & r) noexcept {
			main_loop::run(r, get_time_factor);
		}
	}
}