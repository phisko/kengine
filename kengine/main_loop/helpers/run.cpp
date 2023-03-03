#include "run.hpp"

// stl
#include <chrono>

// entt
#include <entt/entity/registry.hpp>

// meta
#include "putils/meta/concepts/invocable.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_frame.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine main_loop
#include "kengine/main_loop/data/time_modulator.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/main_loop/helpers/is_running.hpp"

namespace kengine::main_loop {
	static constexpr auto log_category = "main_loop";

	template<typename T>
	concept time_factor_callback = putils::invocable<T, float(const entt::registry &)>;

	template<time_factor_callback F>
	static void run_frame(const entt::registry & r, float delta_time, F && get_time_factor) noexcept {
		KENGINE_PROFILING_SCOPE;

		delta_time *= get_time_factor(r);

		kengine_logf(r, very_verbose, log_category, "Calling execute (dt: {})", delta_time);
		for (const auto & [e, func] : r.view<kengine::main_loop::execute>().each()) {
			if (!is_running(r))
				break;
			kengine_logf(r, very_verbose, log_category, "Calling execute on {}", e);
			func(delta_time);
		}
	}

	template<time_factor_callback F>
	static void run(entt::registry & r, F && get_time_factor) noexcept {
		kengine_log(r, log, log_category, "Starting");

		auto previous_time = std::chrono::system_clock::now();
		while (is_running(r)) {
			const auto now = std::chrono::system_clock::now();
			const float delta_time = std::chrono::duration<float>(now - previous_time).count();
			previous_time = now;

			run_frame(r, delta_time, get_time_factor);

			KENGINE_PROFILING_FRAME;
		}

		kengine_log(r, log, log_category, "Stopping due to no more main_loop::keep_alive components remaining");
	}

	void run(entt::registry & r) noexcept {
		run(r, [](auto &&...) noexcept { return 1.f; });
	}

	namespace time_modulated {
		static float get_time_factor(const entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Getting time factor");

			float ret = 1.f;
			for (const auto & [e, time_modulator] : r.view<time_modulator>().each()) {
				kengine_logf(r, very_verbose, log_category, "Found time modulator {} ({})", e, time_modulator.factor);
				ret *= time_modulator.factor;
			}

			kengine_logf(r, very_verbose, log_category, "Final time modulator: {}", ret);
			return ret;
		}

		void run(entt::registry & r) noexcept {
			main_loop::run(r, get_time_factor);
		}
	}
}