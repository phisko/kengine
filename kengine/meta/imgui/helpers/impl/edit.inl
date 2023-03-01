#include "edit.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine imgui
#include "kengine/imgui/helpers/set_context.hpp"

namespace kengine::meta {
	template<typename T>
	void meta_component_implementation<imgui::edit, T>::function(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::imgui::edit", "Editing [%u]'s %s", e.entity(), putils::reflection::get_class_name<T>());

		T * comp = nullptr;

		if constexpr (!std::is_empty<T>())
			comp = e.try_get<T>();
		else {
			kengine_log(*e.registry(), very_verbose, "meta::imgui::edit", "Component is empty, using static instance");
			static T instance;
			comp = &instance;
		}

		if (!kengine::imgui::set_context(*e.registry()))
			return;

		if (comp)
			putils::reflection::imgui_edit(*comp);
		else
			kengine_log(*e.registry(), very_verbose, "meta::imgui::edit", "Component not found");
	}
}