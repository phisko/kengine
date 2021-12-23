#include "helpers/registerTypeHelper.hpp"
#include "meta/MatchString.hpp"

namespace kengine::types{
	void registerkenginemetaMatchString() noexcept {
		kengine::registerComponents<kengine::meta::MatchString>();

	}
}