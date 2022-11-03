#include "helpers/registerTypeHelper.hpp"
#include "data/PythonComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginePythonComponent() noexcept {
#ifdef KENGINE_PYTHON
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::PythonComponent'");
		kengine::registerComponents<kengine::PythonComponent>();
#else
		kengine_log(Log, "Init/registerTypes", "Not registering 'kengine::PythonComponent' because 'KENGINE_PYTHON' is not defined");
#endif
	}
}