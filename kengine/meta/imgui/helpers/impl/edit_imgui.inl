#include "edit_imgui.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::meta {
	template<typename T>
	void meta_component_implementation<edit_imgui, T>::function(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::edit_imgui", "Editing [%u]'s %s", e.entity(), putils::reflection::get_class_name<T>());

		T * comp = nullptr;

		if constexpr (!std::is_empty<T>())
			comp = e.try_get<T>();
		else {
			kengine_log(*e.registry(), very_verbose, "meta::edit_imgui", "Component is empty, using static instance");
			static T instance;
			comp = &instance;
		}

		if (comp)
			putils::reflection::imgui_edit(*comp);
		else
			kengine_log(*e.registry(), very_verbose, "meta::edit_imgui", "Component not found");
	}
}