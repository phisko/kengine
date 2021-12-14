#include "helpers/registerTypeHelper.hpp"
#include "meta/Copy.hpp"

namespace kengine::impl::types{
	void registerkenginemetaCopy() noexcept {
		kengine::registerComponents<kengine::meta::Copy>();

	}
}