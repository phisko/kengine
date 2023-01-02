#include "kengine/helpers/register_type_helper.hpp"
#include "kengine/data/time_modulator.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_time_modulator(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::time_modulator'");
		kengine::register_components<kengine::data::time_modulator>(r);
	}
}