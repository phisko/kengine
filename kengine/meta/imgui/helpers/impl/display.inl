#include "display.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::meta {
	template<typename T>
	void meta_component_implementation<imgui::display, T>::function(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::imgui::display", "Displaying [%u]'s %s", e.entity(), putils::reflection::get_class_name<T>());

		const T * comp = nullptr;

		if constexpr (!std::is_empty<T>())
			comp = e.try_get<T>();
		else {
			kengine_log(*e.registry(), very_verbose, "meta::imgui::display", "Component is empty, using static instance");
			static constexpr T instance;
			comp = &instance;
		}

		if (comp)
			putils::reflection::imgui_edit(*comp);
		else
			kengine_log(*e.registry(), very_verbose, "meta::imgui::display", "Component not found");
	}
}
