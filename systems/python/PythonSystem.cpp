#include "PythonSystem.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/pythonHelper.hpp"
#include "data/PythonComponent.hpp"
#include "functions/Execute.hpp"

namespace kengine {
	EntityCreator * PythonSystem() noexcept {
		static py::module_ * module_ = nullptr;

		struct impl {
			static void init(Entity & e) noexcept {
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
				kengine_log(Verbose, "Execute", "PythonSystem");
				module_->attr("deltaTime") = deltaTime;
				for (auto [e, comp] : entities.with<PythonComponent>()) {
					module_->attr("self") = &e;
					for (const auto & s : comp.scripts) {
						kengine_logf(Verbose, "Execute/PythonSystem", "%zu: %s", e.id, s.c_str());
						py::eval_file(s.c_str(), py::globals());
					}
				}
			}
		};

		return [](Entity & e) noexcept {
			impl::init(e);
		};
	}
}