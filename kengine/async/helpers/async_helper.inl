#include "async_helper.hpp"

// stl
#include <chrono>
#include <concepts>
#include <future>

// kengine core
#include "kengine/core/helpers/log_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine async
#include "kengine/async/data/async_task.hpp"

namespace kengine {
	namespace impl {
		template<typename T>
		struct async_result {
			std::future<T> future;
		};
	}

	template<typename T>
	void start_async_task(entt::registry & r, entt::entity e, const data::async_task::string & task_name, std::future<T> && future) noexcept {
		KENGINE_PROFILING_SCOPE;

		kengine_logf(r, log, "async", "Async task '%s' starting", task_name.c_str());
		r.emplace<data::async_task>(e, task_name);
		r.emplace<impl::async_result<T>>(e, std::move(future));
	}

	template<typename T>
	bool process_async_results(entt::registry & r) noexcept {
		return process_async_results<T>(r, [](auto &&...) {});
	}

	template<typename T, std::invocable<entt::entity, T &&> Func>
	bool process_async_results(entt::registry & r, Func && func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, "async", "Processing async results");

		const auto now = std::chrono::system_clock::now();

		for (const auto & [e, result, task] : r.view<impl::async_result<T>, data::async_task>().each()) {
			const auto time_running = std::chrono::duration<float>(now - task.start).count();

			using namespace std::chrono_literals;
			const auto status = result.future.wait_for(0s);
			if (status == std::future_status::timeout) {
				kengine_logf(r, verbose, "async", "Async task '%s' still running (started %fs ago)", task.name.c_str(), time_running);
				continue;
			}

			kengine_logf(r, log, "async", "Async task '%s' completed after %fs", task.name.c_str(), time_running);

			auto data = result.future.get();

			r.erase<data::async_task>(e);
			r.erase<impl::async_result<T>>(e);

			func(e, std::move(data));
		}

		return r.view<impl::async_result<T>>().empty();
	}
}