#include "kengine/helpers/meta/register_everything.hpp"
#include "putils/point.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_putils_point2f(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'putils::point2f'");
		kengine::register_everything<false, putils::point2f>(r);
	}
}