#include "kengine/helpers/register_type_helper.hpp"
#include "kengine/data/Text.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types{
	void register_kengine_data_text_2d(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::text_2d'");
		kengine::register_components<kengine::data::text_2d>(r);
	}
}