#include "helpers/registerTypeHelper.hpp"
#include "meta/SaveToJSON.hpp"

namespace kengine::types{
	void registerkenginemetaSaveToJSON() noexcept {
		kengine::registerComponents<kengine::meta::SaveToJSON>();

	}
}