#include "helpers/registerTypeHelper.hpp"
#include "meta/AttachTo.hpp"

namespace kengine::impl::types{
	void registerkenginemetaAttachTo() noexcept {
		kengine::registerComponents<kengine::meta::AttachTo>();

	}
}