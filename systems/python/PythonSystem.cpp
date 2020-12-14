#include "PythonSystem.hpp"
#include "helpers/pythonHelper.hpp"
#include "data/PythonComponent.hpp"
#include "functions/Execute.hpp"

namespace kengine::python {
	struct impl {
		static inline EntityManager * em;
		static inline py::module_ * module_;

		static void init(Entity & e) {
			auto tmp = std::make_unique<PythonStateComponent::Data>();
			auto & state = *tmp;
			e += PythonStateComponent{ std::move(tmp) };

			py::globals()["kengine"] = state.module_;
			module_ = &state.module_;
			scriptLanguageHelper::init(*em,
				[&](auto && ... args) {
					pythonHelper::detail::registerFunctionWithState(state, FWD(args)...);
				},
				[&](auto type) {
					using T = putils_wrapped_type(type);
					pythonHelper::detail::registerTypeWithState<T>(*em, state);
				}
			);

			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) {
			module_->attr("deltaTime") = deltaTime;
			for (auto [e, comp] : em->getEntities<PythonComponent>()) {
				module_->attr("self") = &e;
				for (const auto & s : comp.scripts)
					py::eval_file(s.c_str(), py::globals());
			}
		}
	};
}

namespace kengine {
	EntityCreatorFunctor<64> PythonSystem(EntityManager & em) {
		python::impl::em = &em;
		return [&](Entity & e) {
			python::impl::init(e);
		};
	}
}