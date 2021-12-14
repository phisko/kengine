#include "helpers/registerTypeHelper.hpp"
#include "meta/DisplayImGui.hpp"

namespace kengine::impl::types{
	void registerkenginemetaDisplayImGui() noexcept {
		kengine::registerComponents<kengine::meta::DisplayImGui>();

	}
}