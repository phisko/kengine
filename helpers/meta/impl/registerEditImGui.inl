#include "registerEditImGui.hpp"

// putils
#include "reflection_helpers/imgui_helper.hpp"

// kengine meta
#include "meta/EditImGui.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerEditImGui() noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::EditImGui, Comps...>(
			[](const auto t, Entity & e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using Comp = putils_wrapped_type(t);
				auto comp = e.tryGet<Comp>();
				if (comp)
					putils::reflection::imguiEdit(*comp);
			}
		);
	}
}

