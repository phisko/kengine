#include "registerDisplayImGui.hpp"

// putils
#include "reflection_helpers/imgui_helper.hpp"

// kengine meta
#include "meta/DisplayImGui.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerDisplayImGui() noexcept {
		registerMetaComponentImplementation<meta::DisplayImGui, Comps...>(
			[](const auto t, const Entity & e) noexcept {
				using Comp = putils_wrapped_type(t);
				const Comp * comp = e.tryGet<Comp>();
				if (comp)
					putils::reflection::imguiEdit(*comp);
			}
		);
	}
}

