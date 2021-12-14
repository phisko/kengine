#include "helpers/registerTypeHelper.hpp"
#include "functions/Execute.hpp"

namespace kengine::impl::types{
	void registerkenginefunctionsExecute() noexcept {
		kengine::registerComponents<kengine::functions::Execute>();

	}
}