#include "kengine/helpers/meta/register_everything.hpp"

#ifdef KENGINE_PYTHON
#include "kengine/data/python.hpp"
#endif

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_python(entt::registry & r) noexcept {
#ifdef KENGINE_PYTHON
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::python'");
		kengine::register_everything<kengine::data::python>(r);
#else
		kengine_log(r, log, "init/register_types", "Not registering 'kengine::data::python' because 'KENGINE_PYTHON' is not defined");
#endif
	}
}