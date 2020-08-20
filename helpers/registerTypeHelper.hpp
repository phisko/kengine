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
	void registerComponents(EntityManager & em);

	template<typename ...Types>
	void registerTypes(EntityManager & em);

	template<typename F>
	void registerFunction(EntityManager & em, const char * name, F && func);
}

// Impl
namespace kengine {
	namespace detail {
		template<typename T, typename Func>
		void registerWithLanguage(EntityManager & em, Func && func, const char * name) {
			try {
				func(em);
			}
			catch (const std::exception & e) {
				kengine_assert_failed(em, std::string("[") + name + "] Error registering [" + putils::reflection::get_class_name<T>() + "]: " + e.what());
			}
		}

		template<typename ...Types, typename Func>
		void registerTypes(EntityManager & em, Func && registerWithLanguages) {
			putils::for_each_type<Types...>([&](auto && t) {
				static bool registered = false;
				if (registered)
					return;
				registered = true;

				registerWithLanguages(t);

				using T = putils_wrapped_type(t);
				putils::reflection::for_each_used_type<T>([&](auto && type) {
					using Used = putils_wrapped_type(type);
					kengine::registerTypes<Used>(em);
				});
			});
		}
	}

	template<typename ...Types>
	void registerTypes(EntityManager & em) {
		detail::registerTypes<Types...>(em, [&](auto && t) {
			using T = putils_wrapped_type(t);
			detail::registerWithLanguage<T>(em, pythonHelper::registerType<T>, "Python");
			detail::registerWithLanguage<T>(em, luaHelper::registerType<T>, "Lua");
		});
	}

	template<typename ... Comps>
	void registerComponents(EntityManager & em) {
		detail::registerTypes<Comps...>(em, [&](auto && t) {
			using T = putils_wrapped_type(t);
			typeHelper::getTypeEntity<T>(em) += kengine::NameComponent{ putils::reflection::get_class_name<T>() };
			detail::registerWithLanguage<T>(em, pythonHelper::registerComponent<T>, "Python");
			detail::registerWithLanguage<T>(em, luaHelper::registerComponent<T>, "Lua");
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