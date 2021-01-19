#include "registerEditImGui.hpp"

#include "meta/EditImGui.hpp"
#include "helpers/registerMetaComponentImplementation.hpp"
#include "reflection/imgui_helper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerEditImGui() noexcept {
		registerMetaComponentImplementation<meta::EditImGui, Comps...>(
			[](const auto t, Entity & e) noexcept {
				using Comp = putils_wrapped_type(t);
				auto comp = e.tryGet<Comp>();
				if (comp)
					putils::reflection::imguiEdit(*comp);
			}
		);
	}
}

