#pragma once

#include "EntityManager.hpp"

#include "PythonHelper.hpp"
#include "LuaHelper.hpp"

#include "RegisterComponentFunctions.hpp"
#include "RegisterComponentEditor.hpp"
#include "RegisterComponentMatcher.hpp"
#include "RegisterComponentJSONLoader.hpp"
#include "RegisterComponentEntityIterators.hpp"
#include "RegisterComponentAttributeIterator.hpp"

#include "data/NameComponent.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerComponents(kengine::EntityManager & em);

	template<typename ...Types>
	void registerTypes(EntityManager & em);

	template<typename F>
	void registerFunction(EntityManager & em, const char * name, F && func);

	template<typename ...Types>
	void registerTypes(EntityManager & em) {
		putils::for_each_type<Types...>([&](auto && t) {
			using T = putils_wrapped_type(t);

			try {
				PythonHelper::registerType<T>(em);
				LuaHelper::registerType<T>(em);
			}
			catch (const std::exception & e) {
				std::cerr << putils::termcolor::red <<
					"Error registering [" << putils::reflection::get_class_name<T>() << "]: " << e.what()
					<< putils::termcolor::reset << '\n';
			}

			putils::reflection::for_each_used_type<T>([&](const char *, auto && type) {
				using Used = putils_wrapped_type(type);
				registerTypes<Used>(em);
			});
		});
	}

	template<typename ... Comps>
	void registerComponents(EntityManager & em) {
		registerTypes<Comps...>(em);

		putils::for_each_type<Comps...>([&](auto && t) {
			using T = putils_wrapped_type(t);
			auto type = TypeHelper::getTypeEntity<T>(em);
			type += kengine::NameComponent{
				putils::reflection::get_class_name<T>()
			};
		});

		registerComponentsFunctions<Comps...>(em);
		registerComponentEditors<Comps...>(em);
		registerComponentMatchers<Comps...>(em);
		registerComponentJSONLoaders<Comps...>(em);
		registerComponentEntityIterators<Comps...>(em);
		registerComponentAttributeIterators<Comps...>(em);
	}

	template<typename F>
	void registerFunction(EntityManager & em, const char * name, F && func) {
		LuaHelper::registerFunction(em, name, func);
		PythonHelper::registerFunction(em, name, func);
	}
}