#include "PySystem.hpp"
#include "helpers/PythonHelper.hpp"
#include "data/PyComponent.hpp"
#include "functions/Execute.hpp"

namespace kengine {
	// declarations
	static void execute(EntityManager & em, py::module & module, float deltaTime);
	//
	EntityCreatorFunctor<64> PySystem(EntityManager & em) {
		return [&](Entity & e) {
			auto tmp = std::make_unique<PythonStateComponent::Data>();
			auto & state = *tmp;
			e += PythonStateComponent{ std::move(tmp) };

			py::globals()["kengine"] = state.module;
			ScriptLanguageHelper::init(em,
				[&](auto && ... args) {
					PythonHelper::detail::registerFunctionWithState(state, FWD(args)...);
				},
				[&](auto type) {
					using T = putils_wrapped_type(type);
					PythonHelper::detail::registerTypeWithState<T>(em, state);
				}
			);

			e += functions::Execute{ [&](float deltaTime) { execute(em, state.module, deltaTime); } };
		};
	}

	static void execute(EntityManager & em, py::module & module, float deltaTime) {
		module.attr("deltaTime") = deltaTime;
		for (auto & [e, comp] : em.getEntities<PyComponent>()) {
			module.attr("self") = &e;
			for (const auto & s : comp.scripts)
				py::eval_file(s.c_str(), py::globals());
		}
	}
}