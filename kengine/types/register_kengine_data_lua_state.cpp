#include "kengine/helpers/meta/register_everything.hpp"

#ifdef KENGINE_LUA
#include "kengine/data/lua_state.hpp"
#endif

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_lua_state(entt::registry & r) noexcept {
#ifdef KENGINE_LUA
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::lua_state'");
		kengine::register_everything<kengine::data::lua_state>(r);
#else
		kengine_log(r, log, "init/register_types", "Not registering 'kengine::data::lua_state' because 'KENGINE_LUA' is not defined");
#endif
	}
}