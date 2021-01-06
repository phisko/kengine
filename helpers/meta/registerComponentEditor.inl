#include "registerComponentEditor.hpp"

#include "kengine.hpp"
#include "meta/DisplayImGui.hpp"
#include "meta/EditImGui.hpp"
#include "reflection/imgui_helper.hpp"
#include "helpers/typeHelper.hpp"

namespace kengine {
	namespace impl {
		template<typename Comp>
		static void displayComponent(const Entity & e) noexcept {
			const auto comp = e.tryGet<Comp>();
			if (comp)
				putils::reflection::imguiDisplay(*comp);
		}

		template<typename Comp>
		static void editComponent(Entity & e) noexcept {
			const auto comp = e.tryGet<Comp>();
			if (comp)
				putils::reflection::imguiEdit(*comp);
		}
	}

	template<typename ... Comps>
	void registerComponentEditor() noexcept {
		putils::for_each_type<Comps...>([](auto t) noexcept {
			using Type = putils_wrapped_type(t);
			auto type = typeHelper::getTypeEntity<Type>();
			type += meta::DisplayImGui{ impl::displayComponent<Type> };
			type += meta::EditImGui{ impl::editComponent<Type> };
		});
	}
}
