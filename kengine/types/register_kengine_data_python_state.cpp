#include "kengine/helpers/meta/register_everything.hpp"

#ifdef KENGINE_PYTHON
#include "kengine/data/python_state.hpp"
#endif

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_python_state(entt::registry & r) noexcept {
#ifdef KENGINE_PYTHON
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::python_state'");
		kengine::register_everything<kengine::data::python_state>(r);
#else
		kengine_log(r, log, "init/register_types", "Not registering 'kengine::data::python_state' because 'KENGINE_PYTHON' is not defined");
#endif
	}
}