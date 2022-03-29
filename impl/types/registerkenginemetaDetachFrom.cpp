#include "helpers/registerTypeHelper.hpp"
#include "meta/DetachFrom.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaDetachFrom() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::DetachFrom'");
		kengine::registerComponents<kengine::meta::DetachFrom>();

	}
}