#include "assert_failed.hpp"

// putils
#include "putils/string.hpp"
#include "putils/get_call_stack.hpp"

// kengine core/assert
#include "kengine/core/assert/functions/on_assert_failed.hpp"

// kengine core/log
#include "kengine/core/log/helpers/kengine_log.hpp"

// kengine core/profiling
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::core::assert {
	static constexpr auto log_category = "core_assert";

	bool assert_failed(const entt::registry & r, const char * file, int line, const char * expr) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, error, log_category, "{}:{} {}\nCallstack:\n{}", file, line, expr, putils::get_call_stack(1));

		const auto view = r.view<on_assert_failed>();
		if (view.empty())
			return true;

		bool ret = false;
		for (const auto & [e, handler] : view.each())
			ret |= handler(file, line, expr);
		return ret;
	}
}