#include "helpers/registerTypeHelper.hpp"
#include "data/PythonComponent.hpp"

namespace kengine::types{
	void registerkenginePythonComponent() noexcept {
		kengine::registerComponents<kengine::PythonComponent>();

	}
}