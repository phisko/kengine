#include "process_results.hpp"

// stl
#include <chrono>

// fmt
#include <fmt/chrono.h>

// kengine
#include "kengine/async/data/result.hpp"
#include "kengine/async/data/task.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::async {
	static constexpr auto log_category = "async";

	template<typename T>
	bool process_results(entt::registry & r) noexcept {
		return process_results<T>(r, [](auto &&...) {});
	}

	template<typename T, std::invocable<entt::entity, T &&> Func>
	bool process_results(entt::registry & r, Func && func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, log_category, "Processing async results");

		const auto now = std::chrono::system_clock::now();

		for (const auto & [e, async_result, async_task] : r.view<result<T>, task>().each()) {
			const auto time_running = now - async_task.start;

			using namespace std::chrono_literals;
			const auto status = async_result.future.wait_for(0s);
			if (status == std::future_status::timeout) {
				kengine_logf(r, verbose, log_category, "Async task '{}' still running (started {:%S}s ago)", async_task.name, time_running);
				continue;
			}

			kengine_logf(r, log, log_category, "Async task '{}' completed after {:%S}s", async_task.name, time_running);

			auto data = async_result.future.get();

			r.erase<task>(e);
			r.erase<result<T>>(e);

			func(e, std::move(data));
		}

		return r.view<result<T>>().empty();
	}
}