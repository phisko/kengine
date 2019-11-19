#pragma once

#include "EntityManager.hpp"
#include "functions/ImGuiEditor.hpp"
#include "reflection/imgui_helper.hpp"

namespace kengine {
	template<typename Comp>
	void registerComponentEditor(kengine::EntityManager & em);

	template<typename ... Comps>
	void registerComponentEditors(kengine::EntityManager & em);
}

namespace kengine {
	namespace detail {
		template<typename Comp>
		static void displayComponent(const kengine::Entity & e) {
			if constexpr (putils::reflection::has_attributes<Comp>::value)
				putils::reflection::imguiDisplay(e.get<Comp>());
		}

		template<typename Comp>
		static void editComponent(kengine::Entity & e) {
			if constexpr (putils::reflection::has_attributes<Comp>::value)
				putils::reflection::imguiEdit(e.get<Comp>());
		}
	}

	template<typename Comp>
	void registerComponentEditor(kengine::EntityManager & em) {
		em.registerComponentFunction<Comp>(functions::DisplayImGui{ detail::displayComponent<Comp> });
		em.registerComponentFunction<Comp>(functions::EditImGui{ detail::editComponent<Comp> });
	}

	template<typename ... Comps>
	void registerComponentEditors(kengine::EntityManager & em) {
		putils_for_each_type(Comps, [&](auto type) {
			using Type = putils_wrapped_type(type);
			registerComponentEditor<Type>(em);
		});
	}
}