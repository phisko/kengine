#include "helpers/registerTypeHelper.hpp"
#include "meta/DisplayImGui.hpp"

namespace kengine::types{
	void registerkenginemetaDisplayImGui() noexcept {
		kengine::registerComponents<kengine::meta::DisplayImGui>();

	}
}