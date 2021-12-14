#include "helpers/registerTypeHelper.hpp"
#include "data/PythonComponent.hpp"

namespace kengine::impl::types{
	void registerkenginePythonComponent() noexcept {
		kengine::registerComponents<kengine::PythonComponent>();

	}
}