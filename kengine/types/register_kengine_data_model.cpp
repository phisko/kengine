#include "kengine/helpers/register_type_helper.hpp"
#include "kengine/data/model.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types{
	void register_kengine_data_model(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::model'");
		kengine::register_components<kengine::data::model>(r);
	}
}