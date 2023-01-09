#include "kengine/helpers/meta/register_everything.hpp"

#ifdef KENGINE_GLM
#include "kengine/data/skeleton.hpp"
#endif

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_skeleton(entt::registry & r) noexcept {
#ifdef KENGINE_GLM
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::skeleton'");
		kengine::register_everything<true, kengine::data::skeleton>(r);
#else
		kengine_log(r, log, "init/register_types", "Not registering 'kengine::data::skeleton' because 'KENGINE_GLM' is not defined");
#endif
	}
}