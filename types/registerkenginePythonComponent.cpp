#include "helpers/registerTypeHelper.hpp"
#include "data/PythonComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginePythonComponent(entt::registry & r) noexcept {
#ifdef KENGINE_PYTHON
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::PythonComponent'");
		kengine::registerComponents<kengine::PythonComponent>(r);
#else
		kengine_log(r, Log, "Init/registerTypes", "Not registering 'kengine::PythonComponent' because 'KENGINE_PYTHON' is not defined");
#endif
	}
}