#include "helpers/registerTypeHelper.hpp"
#include "meta/Attributes.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaAttributes() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Attributes'");
		kengine::registerComponents<kengine::meta::Attributes>();
	}
}