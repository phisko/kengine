#include "helpers/registerTypeHelper.hpp"
#include "data/PythonComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginePythonComponent() noexcept {
#ifdef KENGINE_PYTHON
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::PythonComponent'");
		kengine::registerComponents<kengine::PythonComponent>();
#else
		kengine_log(Log, "Init/registerTypes", "Not registering 'kengine::PythonComponent' because 'KENGINE_PYTHON' is not defined");
#endif
	}
}