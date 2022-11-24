#include "PythonSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "forward_to.hpp"

// kengine data
#include "data/PythonComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"
#include "helpers/pythonHelper.hpp"

namespace kengine {
	struct PythonSystem {
		entt::registry & r;
		py::module_ * module_;

		PythonSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "PythonSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));

			kengine_log(r, Log, "Init/Python", "Creating PythonStateComponent");
			auto state = new PythonStateComponent::Data;
			e.emplace<PythonStateComponent>(state);

			kengine_log(r, Log, "Init/Python", "Registering scriptLanguageHelper functions");
			py::globals()["kengine"] = state->module_;
			module_ = &state->module_;
			scriptLanguageHelper::init(
				r,
				[&](auto && ... args) noexcept {
					pythonHelper::impl::registerFunctionWithState(*state, FWD(args)...);
				},
				[&](auto type) noexcept {
					using T = putils_wrapped_type(type);
					pythonHelper::impl::registerTypeWithState<T>(*state);
				}
			);
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute", "PythonSystem");

			module_->attr("deltaTime") = deltaTime;

			for (auto [e, comp] : r.view<PythonComponent>().each()) {
				module_->attr("self") = entt::handle{ r, e };

				for (const auto & s : comp.scripts) {
					kengine_logf(r, Verbose, "Execute/PythonSystem", "%zu: %s", e, s.c_str());
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

	void addPythonSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<PythonSystem>(e);
	}
}