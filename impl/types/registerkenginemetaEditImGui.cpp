#include "helpers/registerTypeHelper.hpp"
#include "meta/EditImGui.hpp"

namespace kengine::impl::types{
	void registerkenginemetaEditImGui() noexcept {
		kengine::registerComponents<kengine::meta::EditImGui>();

	}
}