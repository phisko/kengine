#ifndef KENGINE_NDEBUG

#include "assert_helper.hpp"

// putils
#include "putils/string.hpp"
#include "putils/get_call_stack.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::assert_helper {
	std::function<bool(const entt::registry & r, const char * file, int line, const std::string & expr)> assert_handler = nullptr;

	bool assert_failed(const entt::registry & r, const char * file, int line, const std::string & expr) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, error, "assert", "%s:%d %s\nCallstack:\n%s", file, line, expr.c_str(), putils::get_call_stack().c_str());

		if (assert_handler)
			return assert_handler(r, file, line, expr);
		return true;
	}

	bool is_debugger_present() noexcept {
		KENGINE_PROFILING_SCOPE;
#ifdef WIN32
		return IsDebuggerPresent();
#else
		return false;
#endif
	}
}

#endif
