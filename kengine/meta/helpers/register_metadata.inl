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

// kengine meta
#include "kengine/meta/data/attributes.hpp"
#include "kengine/meta/data/size.hpp"
#include "kengine/meta/helpers/type_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_metadata(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "register_metadata", "Registering metadata");

		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			KENGINE_PROFILING_SCOPE;

			using type = putils_wrapped_type(t);
			kengine_logf(r, verbose, "register_metadata", "Registering metadata for %s", putils::reflection::get_class_name<type>());

			const auto e = type_helper::get_type_entity<type>(r);
			r.emplace<meta::attributes>(e, &putils::reflection::runtime::get_attributes<type>());
			r.emplace<data::name>(e, putils::reflection::get_class_name<type>());
			r.emplace<meta::size>(e, sizeof(type));
		});
	}
}