#include "PythonSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

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
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "PythonSystem");

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);

			kengine_log(r, Log, "Init/Python", "Creating PythonStateComponent");
			auto state = new PythonStateComponent::Data;
			r.emplace<PythonStateComponent>(e, state);

			kengine_log(r, Log, "Init/Python", "Registering scriptLanguageHelper functions");
			py::globals()["kengine"] = state->module_;
			_module = &state->module_;
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

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Verbose, "Execute", "PythonSystem");

			_module->attr("deltaTime") = deltaTime;

			for (auto [e, comp] : _r->view<PythonComponent>().each()) {
				_module->attr("self") = entt::handle{ *_r, e };

				for (const auto & s : comp.scripts) {
					kengine_logf(*_r, Verbose, "Execute/PythonSystem", "%zu: %s", e, s.c_str());
					try {
						py::eval_file(s.c_str(), py::globals());
					}
					catch (const std::exception & e) {
						kengine_assert_failed(*_r, e.what());
					}
				}
			}
		}

		static inline entt::registry * _r = nullptr;
		static inline py::module_ * _module = nullptr;
	};

	void PythonSystem(entt::registry & r) noexcept {
		PythonSystem::init(r);
	}
}