#include "register_with_script_languages.hpp"

// stl
#include <algorithm>
#include <execution>

// putils
#include "putils/range.hpp"
#include "putils/thread_name.hpp"

// kengine helpers
#include "kengine/helpers/lua_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"
#include "kengine/helpers/python_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_component_with_script_languages(entt::registry & r) noexcept {
		kengine_log(r, verbose, "register_storage", "Registering components");
		register_with_script_languages<true, Comps...>(r);
	}

	template<typename... Comps>
	void register_type_with_script_languages(entt::registry & r) noexcept {
		kengine_log(r, verbose, "register_storage", "Registering non-components");
		register_with_script_languages<false, Comps...>(r);
	}

	template<bool IsComponent, typename... Comps>
	void register_with_script_languages(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "register_storage", "Registering types");

		using language_registrator = void(entt::registry &);
		static constexpr language_registrator * language_registrators[] = {
			nullptr, // Needed to avoid zero-sized array
#ifdef KENGINE_PYTHON
			python_helper::register_types<IsComponent, Comps...>,
#endif
#ifdef KENGINE_LUA
			lua_helper::register_types<IsComponent, Comps...>,
#endif
		};

		std::for_each(std::execution::par_unseq, putils_range(language_registrators), [&](const auto func) {
			const putils::scoped_thread_name thread_name("Script language type registration");
			if (func)
				func(r);
		});

		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);
			kengine_logf(r, verbose, "register_with_script_languages", "Registering used types for %s", putils::reflection::get_class_name<type>());
			putils::reflection::for_each_used_type<type>([&](auto && t) noexcept {
				using used_type = putils_wrapped_type(t.type);
				kengine_logf(r, verbose, "register_with_script_languages", "Registering %s", putils::reflection::get_class_name<used_type>());
				register_with_script_languages<false, used_type>(r);
			});
		});
	}

	template<typename F>
	void register_with_script_languages(const entt::registry & r, const char * name, F && func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, verbose, "register_with_script_languages", "Registering function %s", name);

#ifdef KENGINE_PYTHON
		python_helper::register_function(r, name, func);
#endif

#ifdef KENGINE_LUA
		lua_helper::register_function(r, name, func);
#endif
	}
}
