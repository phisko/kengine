#include "display_imgui.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine core
#include "kengine/core/helpers/log_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename T>
	void meta_component_implementation<meta::display_imgui, T>::function(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::display_imgui", "Displaying [%u]'s %s", e.entity(), putils::reflection::get_class_name<T>());

		const T * comp = nullptr;

		if constexpr (!std::is_empty<T>())
			comp = e.try_get<T>();
		else {
			kengine_log(*e.registry(), very_verbose, "meta::display_imgui", "Component is empty, using static instance");
			static constexpr T instance;
			comp = &instance;
		}

		if (comp)
			putils::reflection::imgui_edit(*comp);
		else
			kengine_log(*e.registry(), very_verbose, "meta::display_imgui", "Component not found");
	}
}
