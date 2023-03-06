#include "edit.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

#ifdef KENGINE_IMGUI
#include "kengine/imgui/helpers/set_context.hpp"
#endif

namespace kengine::meta {
	template<typename T>
	bool meta_component_implementation<imgui::edit, T>::function(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::imgui::edit", "Editing {}'s {}", e, putils::reflection::get_class_name<T>());

		T * comp = nullptr;

		if constexpr (!std::is_empty<T>())
			comp = e.try_get<T>();
		else {
			kengine_log(*e.registry(), very_verbose, "meta::imgui::edit", "Component is empty, using static instance");
			static T instance;
			comp = &instance;
		}

#ifdef KENGINE_IMGUI
		if (!kengine::imgui::set_context(*e.registry()))
			return false;
#endif

		if (!comp) {
			kengine_log(*e.registry(), very_verbose, "meta::imgui::edit", "Component not found");
			return false;
		}

		if (putils::reflection::imgui_edit(*comp)) {
			kengine_logf(*e.registry(), verbose, "meta::imgui::edit", "Modified {}'s {}", e, putils::reflection::get_class_name<T>());
			return true;
		}

		return false;
	}
}