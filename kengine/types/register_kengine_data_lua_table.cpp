#include "kengine/helpers/meta/register_everything.hpp"

#ifdef KENGINE_LUA
#include "kengine/data/lua_table.hpp"
#endif

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::types {
	void register_kengine_data_lua_table(entt::registry & r) noexcept {
#ifdef KENGINE_LUA
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, log, "init/register_types", "Registering 'kengine::data::lua_table'");
		kengine::register_everything<true, kengine::data::lua_table>(r);
#else
		kengine_log(r, log, "init/register_types", "Not registering 'kengine::data::lua_table' because 'KENGINE_LUA' is not defined");
#endif
	}
}