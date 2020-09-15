#pragma once

#include "EntityManager.hpp"
#include "meta/DisplayImGui.hpp"
#include "meta/EditImGui.hpp"
#include "reflection/imgui_helper.hpp"
#include "helpers/typeHelper.hpp"

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
			const auto comp = e.tryGet<Comp>();
			if (comp)
				putils::reflection::imguiDisplay(*comp);
		}

		template<typename Comp>
		static void editComponent(Entity & e) {
			const auto comp = e.tryGet<Comp>();
			if (comp)
				putils::reflection::imguiEdit(*comp);
		}
	}

	template<typename Comp>
	void registerComponentEditor(EntityManager & em) {
		auto type = typeHelper::getTypeEntity<Comp>(em);
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