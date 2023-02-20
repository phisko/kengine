#include "is_debugger_present.hpp"

// windows
#ifdef _WIN32
#include <windows.h>
#endif

// kengine core/profiling
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::core::assert {
	bool is_debugger_present() noexcept {
		KENGINE_PROFILING_SCOPE;
#ifdef WIN32
		return IsDebuggerPresent();
#else
		return false;
#endif
	}
}