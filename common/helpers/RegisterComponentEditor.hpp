#pragma once

#include "EntityManager.hpp"
#include "functions/ImGuiEditor.hpp"
#include "reflection/imgui_helper.hpp"
#include "helpers/TypeHelper.hpp"

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
			putils::reflection::imguiDisplay(e.get<Comp>());
		}

		template<typename Comp>
		static void editComponent(kengine::Entity & e) {
			putils::reflection::imguiEdit(e.get<Comp>());
		}
	}

	template<typename Comp>
	void registerComponentEditor(kengine::EntityManager & em) {
		auto type = TypeHelper::getTypeEntity<Comp>(em);
		type += functions::DisplayImGui{ detail::displayComponent<Comp> };
		type += functions::EditImGui{ detail::editComponent<Comp> };
	}

	template<typename ... Comps>
	void registerComponentEditors(kengine::EntityManager & em) {
		putils::for_each_type<Comps...>([&](auto type) {
			using Type = putils_wrapped_type(type);
			registerComponentEditor<Type>(em);
		});
	}
}