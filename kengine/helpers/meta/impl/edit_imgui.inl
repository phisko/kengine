#include "edit_imgui.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine helpers
#include "kengine/helpers/imgui_edit_entity.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename T>
	void meta_component_implementation<meta::edit_imgui, T>::function(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;

		T * comp = nullptr;

		if constexpr (!std::is_empty<T>())
			comp = e.try_get<T>();
		else {
			static T instance;
			comp = &instance;
		}

		if (comp)
			putils::reflection::imgui_edit(*comp);
	}
}