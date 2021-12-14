#include "helpers/registerTypeHelper.hpp"
#include "meta/Attributes.hpp"

namespace kengine::impl::types{
	void registerkenginemetaAttributes() noexcept {
		kengine::registerComponents<kengine::meta::Attributes>();

	}
}