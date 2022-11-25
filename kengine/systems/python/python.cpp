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
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/python");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			kengine_log(r, log, "Init/systems/python", "Creating data::python_state");
			auto & state = e.emplace<data::python_state>();

			kengine_log(r, log, "Init/systems/python", "Registering script_language_helper functions");
			py::globals()["kengine"] = state.module_;
			module_ = &state.module_;
			script_language_helper::init(
				r,
				[&](auto && ... args) noexcept {
					python_helper::impl::register_function_with_state(state, FWD(args)...);
				},
				[&](auto type) noexcept {
					using T = putils_wrapped_type(type);
					python_helper::impl::register_type_with_state<T>(state);
				}
			);
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "execute", "python");

			module_->attr("delta_time") = delta_time;

			for (auto [e, comp] : r.view<data::python>().each()) {
				module_->attr("self") = entt::handle{ r, e };

				for (const auto & s : comp.scripts) {
					kengine_logf(r, verbose, "execute/python", "%zu: %s", e, s.c_str());
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

	void add_python(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<python>(e);
	}
}