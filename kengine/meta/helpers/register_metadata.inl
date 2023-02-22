#pragma once

#include "register_metadata.hpp"

// entt
#include <entt/entity/registry.hpp>

// meta
#include "putils/meta/for_each.hpp"

// putils
#include "putils/reflection_helpers/runtime_helper.hpp"

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine meta
#include "kengine/meta/data/attributes.hpp"
#include "kengine/meta/data/size.hpp"
#include "kengine/meta/helpers/get_type_entity.hpp"

namespace kengine::meta {
	template<typename... Comps>
	void register_metadata(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "meta", "Registering metadata");

		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			KENGINE_PROFILING_SCOPE;

			using type = putils_wrapped_type(t);
			kengine_logf(r, verbose, "meta", "Registering metadata for %s", putils::reflection::get_class_name<type>());

			const auto e = get_type_entity<type>(r);
			r.emplace<meta::attributes>(e, &putils::reflection::runtime::get_attributes<type>());
			r.emplace<core::name>(e, putils::reflection::get_class_name<type>());
			r.emplace<meta::size>(e, sizeof(type));
		});
	}
}