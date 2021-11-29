#include "registerDisplayImGui.hpp"

#include "meta/DisplayImGui.hpp"
#include "helpers/registerMetaComponentImplementation.hpp"
#include "reflection_helpers/imgui_helper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerDisplayImGui() noexcept {
		registerMetaComponentImplementation<meta::DisplayImGui, Comps...>(
			[](const auto t, const Entity & e) noexcept {
				using Comp = putils_wrapped_type(t);
				const auto comp = e.tryGet<Comp>();
				if (comp)
					putils::reflection::imguiDisplay(*comp);
			}
		);
	}
}

