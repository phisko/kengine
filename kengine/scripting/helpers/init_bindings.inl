#include "init_bindings.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

#ifdef KENGINE_MAIN_LOOP
#include "kengine/main_loop/helpers/stop_running.hpp"
#endif

#include "entt_handle_reflection.hpp"
#include "log_category.hpp"

namespace kengine::scripting {
	template<typename Func, typename Func2>
	void init_bindings(entt::registry & r, Func && register_function, Func2 && register_type) noexcept {
		KENGINE_PROFILING_SCOPE;

		kengine_log(r, verbose, log_category, "Registering function create_entity");
		register_function(
			"create_entity",
			std::function<entt::handle()>(
				[&] { return entt::handle{ r, r.create() }; }
			)
		);

		kengine_log(r, verbose, log_category, "Registering function destroy_entity");
		register_function(
			"destroy_entity",
			std::function<void(entt::handle)>(
				[](entt::handle e) { e.destroy(); }
			)
		);

		kengine_log(r, verbose, log_category, "Registering function for_each_entity");
		using ForEachEntityCallback = std::function<void(entt::handle)>;
		register_function(
			"for_each_entity",
			std::function<void(const ForEachEntityCallback &)>(
				[&](const ForEachEntityCallback & f) {
					r.each([&](entt::entity e) {
						f({ r, e });
					});
				}
			)
		);

#ifdef KENGINE_MAIN_LOOP
		kengine_log(r, verbose, log_category, "Registering function stop_running");
		register_function(
			"stop_running",
			std::function<void()>(
				[&] {
					main_loop::stop_running(r);
				}
			)
		);
#else
		kengine_log(r, verbose, log_category, "Not registering function stop_running because KENGINE_MAIN_LOOP is not defined");
#endif

		kengine_log(r, verbose, log_category, "Registering type entt::handle");
		register_type(putils::meta::type<entt::handle>{});
	}
}