#include "PythonSystem.hpp"
#include "helpers/pythonHelper.hpp"
#include "data/PythonComponent.hpp"
#include "functions/Execute.hpp"

namespace kengine {
#pragma region declarations
	static void execute(EntityManager & em, py::module & module, float deltaTime);
#pragma endregion
	EntityCreatorFunctor<64> PythonSystem(EntityManager & em) {
		return [&](Entity & e) {
			auto tmp = std::make_unique<PythonStateComponent::Data>();
			auto & state = *tmp;
			e += PythonStateComponent{ std::move(tmp) };

			py::globals()["kengine"] = state.module;
			scriptLanguageHelper::init(em,
				[&](auto && ... args) {
					pythonHelper::detail::registerFunctionWithState(state, FWD(args)...);
				},
				[&](auto type) {
					using T = putils_wrapped_type(type);
					pythonHelper::detail::registerTypeWithState<T>(em, state);
				}
			);

			e += functions::Execute{ [&](float deltaTime) { execute(em, state.module, deltaTime); } };
		};
	}

	static void execute(EntityManager & em, py::module & module, float deltaTime) {
		module.attr("deltaTime") = deltaTime;
		for (auto & [e, comp] : em.getEntities<PythonComponent>()) {
			module.attr("self") = &e;
			for (const auto & s : comp.scripts)
				py::eval_file(s.c_str(), py::globals());
		}
	}
}