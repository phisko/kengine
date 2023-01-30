#pragma once

#include "register_metadata.hpp"

// entt
#include <entt/entity/registry.hpp>

// meta
#include "putils/meta/for_each.hpp"

// kengine data
#include "kengine/data/name.hpp"

// kengine meta
#include "kengine/meta/size.hpp"

// kengine helpers
#include "kengine/helpers/type_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_metadata(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			KENGINE_PROFILING_SCOPE;

			using type = putils_wrapped_type(t);

			const auto e = type_helper::get_type_entity<type>(r);
			r.storage<type>(); // Pre-instantiate the component pool
			r.emplace<data::name>(e, putils::reflection::get_class_name<type>());
			r.emplace<meta::size>(e, sizeof(type));
		});
	}
}
