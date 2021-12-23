#include "helpers/registerTypeHelper.hpp"
#include "meta/AttachTo.hpp"

namespace kengine::types{
	void registerkenginemetaAttachTo() noexcept {
		kengine::registerComponents<kengine::meta::AttachTo>();

	}
}