#include "register_function.hpp"

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/scripting/python/data/state.hpp"

#include "log_category.hpp"

namespace kengine::scripting::python {
	namespace impl {
		template<typename Ret, typename... Args>
		void register_function_with_state(state & state, const char * name, const std::function<Ret(Args...)> & func) noexcept {
			KENGINE_PROFILING_SCOPE;

			if constexpr (std::is_reference_v<Ret>)
				state.module_.def(name, func, py::return_value_policy::reference);
			else
				state.module_.def(name, func);
		}
	}

	template<typename Ret, typename... Args>
	void register_function(const entt::registry & r, const char * name, const std::function<Ret(Args...)> & func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, log, log_category, "Registering function %s", name);

		for (const auto & [e, comp] : r.view<state>().each())
			impl::register_function_with_state(comp, name, func);
	}
}