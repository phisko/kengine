#pragma once

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "kengine/data/name.hpp"

// kengine meta
#include "kengine/meta/size.hpp"

// kengine helpers
#include "kengine/helpers/python_helper.hpp"
#include "kengine/helpers/lua_helper.hpp"
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"
#include "kengine/helpers/meta/impl/register_attributes.hpp"
#include "kengine/helpers/meta/impl/register_count.hpp"
#include "kengine/helpers/meta/impl/register_display_imgui.hpp"
#include "kengine/helpers/meta/impl/register_edit_imgui.hpp"
#include "kengine/helpers/meta/impl/register_emplace_or_replace.hpp"
#include "kengine/helpers/meta/impl/register_for_each_entity.hpp"
#include "kengine/helpers/meta/impl/register_get.hpp"
#include "kengine/helpers/meta/impl/register_has.hpp"
#include "kengine/helpers/meta/impl/register_load_from_json.hpp"
#include "kengine/helpers/meta/impl/register_match_string.hpp"
#include "kengine/helpers/meta/impl/register_remove.hpp"
#include "kengine/helpers/meta/impl/register_save_to_json.hpp"

namespace kengine {
	namespace impl {
		template<typename T, typename Registry, typename Callback>
		void register_with_language(Registry && r, Callback && func, const char * name) noexcept {
			KENGINE_PROFILING_SCOPE;

			try {
				func(FWD(r));
			}
			catch (const std::exception & e) {
				kengine_assert_failed(r, "[", name, "] Error registering [", putils::reflection::get_class_name<T>(), "]: ", e.what());
			}
		}

		template<typename T>
		struct registered {}; // Tag to mark already registered types

		template<typename... Types, typename Func>
		void register_types(entt::registry & r, Func && register_with_languages) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::for_each_type<Types...>([&](auto && t) noexcept {
				using type = putils_wrapped_type(t);

				// Avoid double registration
				if (!r.view<registered<type>>().empty())
					return;
				const auto e = r.create();
				r.emplace<registered<type>>(e);

				register_with_languages(t);

				putils::reflection::for_each_used_type<type>([&](const auto & type) noexcept {
					using Used = putils_wrapped_type(type.type);
					kengine::register_types<Used>(r);
				});
			});
		}
	}

	template<typename... Types>
	void register_types(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		impl::register_types<Types...>(r, [&](auto && t) noexcept {
			using type = putils_wrapped_type(t);
#ifdef KENGINE_PYTHON
			impl::register_with_language<type>(r, python_helper::register_types<type>, "python");
#endif

#ifdef KENGINE_LUA
			impl::register_with_language<type>(r, lua_helper::register_types<type>, "lua");
#endif
		});
	}

	template<typename... Comps>
	void register_components(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		impl::register_types<Comps...>(r, [&](auto && t) noexcept {
			using type = putils_wrapped_type(t);

			const auto e = type_helper::get_type_entity<type>(r);
			r.emplace<data::name>(e, putils::reflection::get_class_name<type>());
			r.emplace<meta::size>(e, sizeof(type));

#ifdef KENGINE_PYTHON
			impl::register_with_language<type>(r, python_helper::register_components<type>, "python");
#endif

#ifdef KENGINE_LUA
			impl::register_with_language<type>(r, lua_helper::register_components<type>, "lua");
#endif
		});

		register_attributes<Comps...>(r);
		register_count<Comps...>(r);
		register_display_imgui<Comps...>(r);
		register_edit_imgui<Comps...>(r);
		register_emplace_or_replace<Comps...>(r);
		register_for_each_entity<Comps...>(r);
		register_get<Comps...>(r);
		register_has<Comps...>(r);
		register_load_from_json<Comps...>(r);
		register_match_string<Comps...>(r);
		register_remove<Comps...>(r);
		register_save_to_json<Comps...>(r);
	}

	template<typename F>
	void register_function(const entt::registry & r, const char * name, F && func) noexcept {
		KENGINE_PROFILING_SCOPE;

#ifdef KENGINE_PYTHON
		python_helper::register_function(r, name, func);
#endif

#ifdef KENGINE_LUA
		lua_helper::register_function(r, name, func);
#endif
	}
}
