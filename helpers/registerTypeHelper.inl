#pragma once

// kengine data
#include "data/NameComponent.hpp"

// kengine meta
#include "meta/Size.hpp"

// kengine helpers
#include "helpers/pythonHelper.hpp"
#include "helpers/luaHelper.hpp"
#include "helpers/assertHelper.hpp"
#include "helpers/profilingHelper.hpp"
#include "helpers/meta/impl/registerAttachTo.hpp"
#include "helpers/meta/impl/registerAttributes.hpp"
#include "helpers/meta/impl/registerCopy.hpp"
#include "helpers/meta/impl/registerCount.hpp"
#include "helpers/meta/impl/registerDetachFrom.hpp"
#include "helpers/meta/impl/registerDisplayImGui.hpp"
#include "helpers/meta/impl/registerEditImGui.hpp"
#include "helpers/meta/impl/registerForEachEntity.hpp"
#include "helpers/meta/impl/registerGet.hpp"
#include "helpers/meta/impl/registerHas.hpp"
#include "helpers/meta/impl/registerLoadFromJSON.hpp"
#include "helpers/meta/impl/registerMatchString.hpp"
#include "helpers/meta/impl/registerMove.hpp"
#include "helpers/meta/impl/registerSaveToJSON.hpp"

namespace kengine {
	namespace impl {
		template<typename T>
		void registerWithLanguage(void (*func)(), const char * name) noexcept {
			KENGINE_PROFILING_SCOPE;

			try {
				func();
			}
			catch (const std::exception & e) {
				kengine_assert_failed("[", name, "] Error registering [", putils::reflection::get_class_name<T>() , "]: ", e.what());
			}
		}

		template<typename ...Types, typename Func>
		void registerTypes(Func && registerWithLanguages) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::for_each_type<Types...>([&](auto && t) noexcept {
				static bool registered = false;
                entities += [&](Entity & e) {
                    e += functions::OnTerminate{ [&] { registered = false; }};
                };
				if (registered)
					return;
				registered = true;

				registerWithLanguages(t);

				using T = putils_wrapped_type(t);
				putils::reflection::for_each_used_type<T>([](const auto & type) noexcept {
					using Used = putils_wrapped_type(type.type);
					kengine::registerTypes<Used>();
				});
			});
		}
	}

	template<typename ...Types>
	void registerTypes() noexcept {
		KENGINE_PROFILING_SCOPE;

		impl::registerTypes<Types...>([](auto && t) noexcept {
			using T = putils_wrapped_type(t);
#ifdef KENGINE_PYTHON
			impl::registerWithLanguage<T>(pythonHelper::registerTypes<T>, "Python");
#endif

#ifdef KENGINE_LUA
			impl::registerWithLanguage<T>(luaHelper::registerTypes<T>, "Lua");
#endif
		});
	}

	template<typename ... Comps>
	void registerComponents() noexcept {
		KENGINE_PROFILING_SCOPE;

		impl::registerTypes<Comps...>([](auto && t) noexcept {
			using T = putils_wrapped_type(t);
			auto e = typeHelper::getTypeEntity<T>();
			e += NameComponent{ putils::reflection::get_class_name<T>() };
			e += meta::Size{ sizeof(T) };

#ifdef KENGINE_PYTHON
			impl::registerWithLanguage<T>(pythonHelper::registerComponents<T>, "Python");
#endif

#ifdef KENGINE_LUA
			impl::registerWithLanguage<T>(luaHelper::registerComponents<T>, "Lua");
#endif
		});

		registerAttachTo<Comps...>();
		registerAttributes<Comps...>();
		registerCopy<Comps...>();
		registerCount<Comps...>();
		registerDetachFrom<Comps...>();
		registerDisplayImGui<Comps...>();
		registerEditImGui<Comps...>();
		registerForEachEntity<Comps...>();
		registerGet<Comps...>();
		registerHas<Comps...>();
		registerLoadFromJSON<Comps...>();
		registerMatchString<Comps...>();
		registerMove<Comps...>();
		registerSaveToJSON<Comps...>();
	}

	template<typename F>
	void registerFunction(const char * name, F && func) noexcept {
		KENGINE_PROFILING_SCOPE;

#ifdef KENGINE_PYTHON
		pythonHelper::registerFunction(name, func);
#endif

#ifdef KENGINE_LUA
		luaHelper::registerFunction(name, func);
#endif
	}
}
