#include "register_with_script_languages.hpp"

// stl
#include <algorithm>
#include <execution>

// putils
#include "putils/range.hpp"
#include "putils/thread_name.hpp"

// kengine core
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

#ifdef KENGINE_SCRIPTING_LUA
// kengine scripting/lua
#include "kengine/scripting/lua/helpers/lua_helper.hpp"
#endif

#ifdef KENGINE_SCRIPTING_PYTHON
// kengine scripting/python
#include "kengine/scripting/python/helpers/python_helper.hpp"
#endif

namespace kengine::meta {

	template<typename... Comps>
	void register_component_with_script_languages(entt::registry & r) noexcept {
		kengine_log(r, verbose, "meta", "Registering components");
		register_with_script_languages<true, Comps...>(r);
	}

	template<typename... Comps>
	void register_type_with_script_languages(entt::registry & r) noexcept {
		kengine_log(r, verbose, "meta", "Registering non-components");
		register_with_script_languages<false, Comps...>(r);
	}

	template<bool IsComponent, typename... Comps>
	void register_with_script_languages(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "meta", "Registering types");

		using language_registrator = void(entt::registry &);
		static constexpr language_registrator * language_registrators[] = {
			nullptr, // Needed to avoid zero-sized array
#ifdef KENGINE_SCRIPTING_PYTHON
			python_helper::register_types<IsComponent, Comps...>,
#endif
#ifdef KENGINE_SCRIPTING_LUA
			lua_helper::register_types<IsComponent, Comps...>,
#endif
		};

#ifdef KENGINE_SCRIPTING_PYTHON
		r.storage<data::python_state>();
#else
		kengine_log(r, verbose, "meta", "Not registering with Python because KENGINE_SCRIPTING_PYTHON is not defined");
#endif

#ifdef KENGINE_SCRIPTING_LUA
		r.storage<data::lua_state>();
#else
		kengine_log(r, verbose, "meta", "Not registering with Lua because KENGINE_SCRIPTING_LUA is not defined");
#endif

		std::for_each(std::execution::par_unseq, putils_range(language_registrators), [&](const auto func) {
			const putils::scoped_thread_name thread_name("Script language type registration");
			if (func)
				func(r);
		});

		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);
			kengine_logf(r, verbose, "meta", "Registering used types for %s", putils::reflection::get_class_name<type>());
			putils::reflection::for_each_used_type<type>([&](auto && t) noexcept {
				using used_type = putils_wrapped_type(t.type);
				kengine_logf(r, verbose, "meta", "Registering %s", putils::reflection::get_class_name<used_type>());
				register_with_script_languages<false, used_type>(r);
			});
		});
	}

	template<typename F>
	void register_with_script_languages(const entt::registry & r, const char * name, F && func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, verbose, "meta", "Registering function %s", name);

#ifdef KENGINE_SCRIPTING_PYTHON
		python_helper::register_function(r, name, func);
#else
		kengine_log(r, verbose, "meta", "Not registering with Python because KENGINE_SCRIPTING_PYTHON is not defined");
#endif

#ifdef KENGINE_SCRIPTING_LUA
		lua_helper::register_function(r, name, func);
#else
		kengine_log(r, verbose, "meta", "Not registering with Lua because KENGINE_SCRIPTING_LUA is not defined");
#endif
	}
}
