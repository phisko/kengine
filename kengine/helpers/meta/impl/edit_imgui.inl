#include "edit_imgui.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine helpers
#include "kengine/helpers/imgui_edit_entity.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename T>
	void meta_component_implementation<meta::edit_imgui, T>::function(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, "meta::edit_imgui", "Editing [%zu]'s %s", e.entity(), putils::reflection::get_class_name<T>());

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