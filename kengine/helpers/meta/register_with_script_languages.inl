#include "register_with_script_languages.hpp"

// stl
#include <algorithm>
#include <execution>

// putils
#include "putils/range.hpp"

// kengine helpers
#include "kengine/helpers/lua_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"
#include "kengine/helpers/python_helper.hpp"

namespace kengine {
	template<bool IsComponent, typename... Comps>
	void register_with_script_languages(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		using language_registrator = void(entt::registry &);
		static constexpr language_registrator * language_registrators[] = {
#ifdef KENGINE_PYTHON
			python_helper::register_types<IsComponent, Comps...>,
#endif
#ifdef KENGINE_LUA
			lua_helper::register_types<IsComponent, Comps...>,
#endif
		};

		std::for_each(std::execution::par_unseq, putils_range(language_registrators), [&](const auto func) {
			func(r);
		});

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
