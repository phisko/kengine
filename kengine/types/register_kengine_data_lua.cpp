#include "kengine/helpers/register_type_helper.hpp"
#include "kengine/data/lua.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_lua(entt::registry & r) noexcept {
#ifdef KENGINE_LUA
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::lua'");
		kengine::register_components<kengine::data::lua>(r);
#else
		kengine_log(r, log, "init/register_types", "Not registering 'kengine::data::lua' because 'KENGINE_LUA' is not defined");
#endif
	}
}