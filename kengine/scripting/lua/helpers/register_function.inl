#include "register_function.hpp"

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/scripting/lua/data/state.hpp"

#include "log_category.hpp"

namespace kengine::scripting::lua {
	template<typename Func>
	void register_function(const entt::registry & r, const char * name, Func && func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, log, "lua", "Registering function %s", name);
		for (const auto & [e, comp]: r.view<state>().each()) {
			auto & state = *comp.ptr;
			state[name] = func;
		}
	}
}