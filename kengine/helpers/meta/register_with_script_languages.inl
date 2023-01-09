#include "register_with_script_languages.hpp"

// kengine helpers
#include "kengine/helpers/lua_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"
#include "kengine/helpers/python_helper.hpp"

namespace kengine {
	template<bool IsComponent, typename... Comps>
	void register_with_script_languages(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

#ifdef KENGINE_PYTHON
		python_helper::register_types<IsComponent, Comps...>(r);
#endif

#ifdef KENGINE_LUA
		lua_helper::register_types<IsComponent, Comps...>(r);
#endif

		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);
			putils::reflection::for_each_used_type<type>([&](auto && t) noexcept {
				using used_type = putils_wrapped_type(t.type);
				register_with_script_languages<false, used_type>(r);
			});
		});
	}

	template<typename F>
	void register_with_script_languages(const entt::registry & r, const char * name, F && func) noexcept {
		KENGINE_PROFILING_SCOPE;

#ifdef KENGINE_PYTHON
		python_helper::register_function(r, name, func);
#endif

#ifdef KENGINE_LUA
		lua_helper::register_function(r, name, func);
#endif
	}
}
