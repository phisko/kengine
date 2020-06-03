#pragma once

#include "EntityManager.hpp"

#include "pythonHelper.hpp"
#include "luaHelper.hpp"
#include "assertHelper.hpp"

#include "meta/registerComponentFunctions.hpp"
#include "meta/registerComponentEditor.hpp"
#include "meta/registerComponentMatcher.hpp"
#include "meta/registerComponentJSONLoader.hpp"
#include "meta/registerComponentEntityIterators.hpp"
#include "meta/registerComponentAttributeIterator.hpp"

#include "data/NameComponent.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerComponents(kengine::EntityManager & em);

	template<typename ...Types>
	void registerTypes(EntityManager & em);

	template<typename F>
	void registerFunction(EntityManager & em, const char * name, F && func);
}

// Impl
namespace kengine {
	template<typename ...Types>
	void registerTypes(EntityManager & em) {
		putils::for_each_type<Types...>([&](auto && t) {
			using T = putils_wrapped_type(t);

			const auto registerWithLanguage = [&](const auto func, const char * name) {
				try {
					func(em);
				}
				catch (const std::exception & e) {
					kengine_assert_failed(em, std::string("[") + name + "] Error registering [" + putils::reflection::get_class_name<T>() + "] " + e.what());
				}
			};

			registerWithLanguage(pythonHelper::registerType<T>, "Python");
			registerWithLanguage(luaHelper::registerType<T>, "Lua");

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
			auto type = typeHelper::getTypeEntity<T>(em);
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
		luaHelper::registerFunction(em, name, func);
		pythonHelper::registerFunction(em, name, func);
	}
}