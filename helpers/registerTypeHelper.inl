#include "registerTypeHelper.hpp"

#include "kengine.hpp"

#include "pythonHelper.hpp"
#include "luaHelper.hpp"
#include "assertHelper.hpp"

#include "meta/registerComponentFunctions.hpp"
#include "meta/registerComponentEditor.hpp"
#include "meta/registerComponentMatcher.hpp"
#include "meta/registerComponentJSONLoader.hpp"
#include "meta/registerComponentEntityIterator.hpp"
#include "meta/registerComponentAttributeIterator.hpp"

#include "data/NameComponent.hpp"

#include "concat.hpp"

namespace kengine {
	namespace impl {
		template<typename T>
		void registerWithLanguage(void (*func)(), const char * name) noexcept {
			try {
				func();
			}
			catch (const std::exception & e) {
				kengine_assert_failed(putils::concat("[", name, "] Error registering [", putils::reflection::get_class_name<T>(), "]: ", e.what()));
			}
		}

		template<typename ...Types, typename Func>
		void registerTypes(Func && registerWithLanguages) noexcept {
			putils::for_each_type<Types...>([&](auto && t) noexcept {
				static bool registered = false;
				if (registered)
					return;
				registered = true;

				registerWithLanguages(t);

				using T = putils_wrapped_type(t);
				putils::reflection::for_each_used_type<T>([](auto && type) noexcept {
					using Used = putils_wrapped_type(type);
					kengine::registerTypes<Used>();
				});
			});
		}
	}

	template<typename ...Types>
	void registerTypes() noexcept {
		impl::registerTypes<Types...>([](auto && t) noexcept {
			using T = putils_wrapped_type(t);
			impl::registerWithLanguage<T>(pythonHelper::registerTypes<T>, "Python");
			impl::registerWithLanguage<T>(luaHelper::registerTypes<T>, "Lua");
		});
	}

	template<typename ... Comps>
	void registerComponents() noexcept {
		impl::registerTypes<Comps...>([](auto && t) noexcept {
			using T = putils_wrapped_type(t);
			typeHelper::getTypeEntity<T>() += kengine::NameComponent{ putils::reflection::get_class_name<T>() };
			impl::registerWithLanguage<T>(pythonHelper::registerComponents<T>, "Python");
			impl::registerWithLanguage<T>(luaHelper::registerComponents<T>, "Lua");
		});
		registerComponentFunctions<Comps...>();
		registerComponentEditor<Comps...>();
		registerComponentMatcher<Comps...>();
		registerComponentJSONLoader<Comps...>();
		registerComponentEntityIterator<Comps...>();
		registerComponentAttributeIterator<Comps...>();
	}

	template<typename F>
	void registerFunction(const char * name, F && func) noexcept {
		luaHelper::registerFunction(name, func);
		pythonHelper::registerFunction(name, func);
	}
}
