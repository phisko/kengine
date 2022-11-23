#pragma once

// entt
#include <entt/entity/registry.hpp>

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
		template<typename T, typename Registry, typename Callback>
		void registerWithLanguage(Registry && r, Callback && func, const char * name) noexcept {
			KENGINE_PROFILING_SCOPE;

			try {
				func(FWD(r));
			}
			catch (const std::exception & e) {
				kengine_assert_failed(r, "[", name, "] Error registering [", putils::reflection::get_class_name<T>() , "]: ", e.what());
			}
		}

		template<typename T>
		struct Registered{}; // Tag to mark already registered types

		template<typename ...Types, typename Func>
		void registerTypes(entt::registry & r, Func && registerWithLanguages) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::for_each_type<Types...>([&](auto && t) noexcept {
				using T = putils_wrapped_type(t);

				// Avoid double registration
				if (!r.view<Registered<T>>().empty())
					return;
				const auto e = r.create();
				r.emplace<Registered<T>>(e);

				registerWithLanguages(t);

				putils::reflection::for_each_used_type<T>([&](const auto & type) noexcept {
					using Used = putils_wrapped_type(type.type);
					kengine::registerTypes<Used>(r);
				});
			});
		}
	}

	template<typename ...Types>
	void registerTypes(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		impl::registerTypes<Types...>(r, [&](auto && t) noexcept {
			using T = putils_wrapped_type(t);
#ifdef KENGINE_PYTHON
			impl::registerWithLanguage<T>(r, pythonHelper::registerTypes<T>, "Python");
#endif

#ifdef KENGINE_LUA
			impl::registerWithLanguage<T>(r, luaHelper::registerTypes<T>, "Lua");
#endif
		});
	}

	template<typename ... Comps>
	void registerComponents(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		impl::registerTypes<Comps...>(r, [&](auto && t) noexcept {
			using T = putils_wrapped_type(t);

			const auto e = typeHelper::getTypeEntity<T>(r);
			r.emplace<NameComponent>(e, putils::reflection::get_class_name<T>());
			r.emplace<meta::Size>(e, sizeof(T));

#ifdef KENGINE_PYTHON
			impl::registerWithLanguage<T>(r, pythonHelper::registerComponents<T>, "Python");
#endif

#ifdef KENGINE_LUA
			impl::registerWithLanguage<T>(r, luaHelper::registerComponents<T>, "Lua");
#endif
		});

		registerAttachTo<Comps...>(r);
		registerAttributes<Comps...>(r);
		registerCopy<Comps...>(r);
		registerCount<Comps...>(r);
		registerDetachFrom<Comps...>(r);
		registerDisplayImGui<Comps...>(r);
		registerEditImGui<Comps...>(r);
		registerForEachEntity<Comps...>(r);
		registerGet<Comps...>(r);
		registerHas<Comps...>(r);
		registerLoadFromJSON<Comps...>(r);
		registerMatchString<Comps...>(r);
		registerMove<Comps...>(r);
		registerSaveToJSON<Comps...>(r);
	}

	template<typename F>
	void registerFunction(const entt::registry & r, const char * name, F && func) noexcept {
		KENGINE_PROFILING_SCOPE;

#ifdef KENGINE_PYTHON
		pythonHelper::registerFunction(r, name, func);
#endif

#ifdef KENGINE_LUA
		luaHelper::registerFunction(r, name, func);
#endif
	}
}
