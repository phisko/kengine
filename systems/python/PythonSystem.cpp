#include "PythonSystem.hpp"

// kengine data
#include "data/PythonComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"
#include "helpers/pythonHelper.hpp"

namespace kengine {
	EntityCreator * PythonSystem() noexcept {
		KENGINE_PROFILING_SCOPE;

		static py::module_ * module_ = nullptr;

		struct impl {
			static void init(Entity & e) noexcept {
				KENGINE_PROFILING_SCOPE;
				kengine_log(Log, "Init", "PythonSystem");

				e += functions::Execute{ execute };

				kengine_log(Log, "Init/Python", "Creating PythonStateComponent");
				auto tmp = std::make_unique<PythonStateComponent::Data>();
				auto & state = *tmp;
				e += PythonStateComponent{ std::move(tmp) };

				kengine_log(Log, "Init/Python", "Registering scriptLanguageHelper functions");
				py::globals()["kengine"] = state.module_;
				module_ = &state.module_;
				scriptLanguageHelper::init(
					[&](auto && ... args) noexcept {
						pythonHelper::impl::registerFunctionWithState(state, FWD(args)...);
					},
					[&](auto type) noexcept {
						using T = putils_wrapped_type(type);
						pythonHelper::impl::registerTypeWithState<T>(state);
					}
				);
			}

			static void execute(float deltaTime) noexcept {
				KENGINE_PROFILING_SCOPE;
				kengine_log(Verbose, "Execute", "PythonSystem");

				module_->attr("deltaTime") = deltaTime;

				for (auto [e, comp] : entities.with<PythonComponent>()) {
					module_->attr("self") = &e;

					for (const auto & s : comp.scripts) {
						kengine_logf(Verbose, "Execute/PythonSystem", "%zu: %s", e.id, s.c_str());
						try {
							py::eval_file(s.c_str(), py::globals());
						}
						catch (const std::exception & e) {
							kengine_assert_failed(e.what());
						}
					}
				}
			}
		};

		return impl::init;
	}
}