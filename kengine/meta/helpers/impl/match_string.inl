#include "match_string.hpp"

// entt
#include <entt/entity/handle.hpp>

// sol
#ifdef KENGINE_SCRIPTING_LUA
#include <sol/sol.hpp>
#endif

// putils
#include "putils/fmt/fmt.hpp"
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine core
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::meta {
	template<typename T>
	bool meta_component_implementation<match_string, T>::function(entt::const_handle e, const char * str) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::match_string", "Matching {}'s {} against {}", e, putils::reflection::get_class_name<T>(), str);

		if constexpr (std::is_empty<T>()) {
			kengine_log(*e.registry(), very_verbose, "meta::match_string", "Component is empty, returning false");
			return false;
		}
		else {
			const auto comp = e.try_get<T>();
			if (!comp) {
				kengine_log(*e.registry(), very_verbose, "meta::match_string", "Component not found, returning false");
				return false;
			}

			const putils::string<1024> comp_string("{}", *comp);
			return comp_string.find(str) != std::string::npos;
		}
	}
}
