#include "helpers/registerTypeHelper.hpp"
#include "meta/Size.hpp"

namespace kengine::types{
	void registerkenginemetaSize() noexcept {
		kengine::registerComponents<kengine::meta::Size>();

	}
}