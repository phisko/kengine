#include "kengine/helpers/meta/register_everything.hpp"
#include "kengine/meta/emplace_or_replace.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_meta_emplace_or_replace(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::meta::emplace_or_replace'");
		kengine::register_everything<true, kengine::meta::emplace_or_replace>(r);
	}
}