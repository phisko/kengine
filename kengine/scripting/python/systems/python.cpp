#include "python.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/python.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"
#include "kengine/helpers/python_helper.hpp"

namespace kengine::systems {
	struct python {
		entt::registry & r;
		py::module_ * module_;

		python(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "python", "Initializing");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			kengine_log(r, verbose, "python", "Creating Python state");
			auto & state = e.emplace<data::python_state>();

			kengine_log(r, verbose, "python", "Registering script_language_helper functions");
			py::globals()["kengine"] = state.module_;
			module_ = &state.module_;
			script_language_helper::init(
				r,
				[&](auto &&... args) noexcept {
					python_helper::impl::register_function_with_state(state, FWD(args)...);
				},
				[&](auto type) noexcept {
					using T = putils_wrapped_type(type);
					python_helper::impl::register_type_with_state<false, T>(r, state);
				}
			);
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "python", "Executing");

			const auto view = r.view<data::python>();

			if (view.empty())
				return;

			kengine_log(r, very_verbose, "python", "Setting delta_time");
			module_->attr("delta_time") = delta_time;

			for (auto [e, comp] : view.each()) {
				kengine_logf(r, very_verbose, "python", "Setting 'self' to [%u]", e);
				module_->attr("self") = entt::handle{ r, e };

				for (const auto & s : comp.scripts) {
					kengine_logf(r, very_verbose, "python", "Running script %s for [%u]", s.c_str(), e);
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

	DEFINE_KENGINE_SYSTEM_CREATOR(python)
}