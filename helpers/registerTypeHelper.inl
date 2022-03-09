#pragma once

#include "pythonHelper.hpp"
#include "luaHelper.hpp"
#include "assertHelper.hpp"

#include "meta/impl/registerAttachTo.hpp"
#include "meta/impl/registerAttributes.hpp"
#include "meta/impl/registerCopy.hpp"
#include "meta/impl/registerCount.hpp"
#include "meta/impl/registerDetachFrom.hpp"
#include "meta/impl/registerDisplayImGui.hpp"
#include "meta/impl/registerEditImGui.hpp"
#include "meta/impl/registerForEachEntity.hpp"
#include "meta/impl/registerGet.hpp"
#include "meta/impl/registerHas.hpp"
#include "meta/impl/registerLoadFromJSON.hpp"
#include "meta/impl/registerMatchString.hpp"
#include "meta/impl/registerSaveToJSON.hpp"
#include "meta/Size.hpp"

#include "data/NameComponent.hpp"

namespace kengine {
	namespace impl {
		template<typename T>
		void registerWithLanguage(void (*func)(), const char * name) noexcept {
			try {
				func();
			}
			catch (const std::exception & e) {
				kengine_assert_failed("[", name, "] Error registering [", putils::reflection::get_class_name<T>() , "]: ", e.what());
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
				putils::reflection::for_each_used_type<T>([](const auto & type) noexcept {
					using Used = putils_wrapped_type(type.type);
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
			auto e = typeHelper::getTypeEntity<T>();
			e += NameComponent{ putils::reflection::get_class_name<T>() };
			e += meta::Size{ sizeof(T) };
			impl::registerWithLanguage<T>(pythonHelper::registerComponents<T>, "Python");
			impl::registerWithLanguage<T>(luaHelper::registerComponents<T>, "Lua");
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
		registerSaveToJSON<Comps...>();
	}

	template<typename F>
	void registerFunction(const char * name, F && func) noexcept {
		luaHelper::registerFunction(name, func);
		pythonHelper::registerFunction(name, func);
	}
}
