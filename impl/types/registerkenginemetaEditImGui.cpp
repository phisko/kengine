#include "helpers/registerTypeHelper.hpp"
#include "meta/EditImGui.hpp"

namespace kengine::types{
	void registerkenginemetaEditImGui() noexcept {
		kengine::registerComponents<kengine::meta::EditImGui>();

	}
}