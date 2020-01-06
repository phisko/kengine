#pragma once

#include "EntityManager.hpp"
#include "meta/DisplayImGui.hpp"
#include "meta/EditImGui.hpp"
#include "reflection/imgui_helper.hpp"
#include "helpers/TypeHelper.hpp"

namespace kengine {
	template<typename Comp>
	void registerComponentEditor(EntityManager & em);

	template<typename ... Comps>
	void registerComponentEditors(EntityManager & em);
}

namespace kengine {
	namespace detail {
		template<typename Comp>
		static void displayComponent(const Entity & e) {
			if (e.has<Comp>())
				putils::reflection::imguiDisplay(e.get<Comp>());
		}

		template<typename Comp>
		static void editComponent(Entity & e) {
			if (e.has<Comp>())
				putils::reflection::imguiEdit(e.get<Comp>());
		}
	}

	template<typename Comp>
	void registerComponentEditor(EntityManager & em) {
		auto type = TypeHelper::getTypeEntity<Comp>(em);
		type += meta::DisplayImGui{ detail::displayComponent<Comp> };
		type += meta::EditImGui{ detail::editComponent<Comp> };
	}

	template<typename ... Comps>
	void registerComponentEditors(EntityManager & em) {
		putils::for_each_type<Comps...>([&](auto type) {
			using Type = putils_wrapped_type(type);
			registerComponentEditor<Type>(em);
		});
	}
}