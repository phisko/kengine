#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/scripting/helpers/init_bindings.hpp"
#include "kengine/scripting/python/data/scripts.hpp"
#include "kengine/scripting/python/helpers/log_category.hpp"
#include "kengine/scripting/python/helpers/register_types.hpp"

namespace kengine::scripting::python {
	struct system {
		entt::registry & r;
		py::module_ * module_;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			kengine_log(r, verbose, log_category, "Creating Python state");
			auto & state = e.emplace<python::state>();

			kengine_log(r, verbose, log_category, "Registering script_language_helper functions");
			py::globals()["kengine"] = state.module_;
			module_ = &state.module_;
			scripting::init_bindings(
				r,
				[&](auto &&... args) noexcept {
					impl::register_function_with_state(state, FWD(args)...);
				},
				[&](auto type) noexcept {
					using T = putils_wrapped_type(type);
					impl::register_type_with_state<false, T>(r, state);
				}
			);
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			const auto view = r.view<scripts>();

			if (view.empty())
				return;

			kengine_log(r, very_verbose, log_category, "Setting delta_time");
			module_->attr("delta_time") = delta_time;

			for (auto [e, comp] : view.each()) {
				kengine_logf(r, very_verbose, log_category, "Setting 'self' to {}", e);
				module_->attr("self") = entt::handle{ r, e };

				for (const auto & s : comp.files) {
					kengine_logf(r, very_verbose, log_category, "Running script {} for {}", s, e);
					try {
						py::eval_file(s.c_str(), py::globals());
					}
					catch (const std::exception & e) {
						kengine_assert_failed(r, e.what());
					}
				}
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}