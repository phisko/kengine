#pragma once

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/reflection_helpers/runtime_helper.hpp"

// kengine meta
#include "kengine/meta/attributes.hpp"

// kengine helpers
#include "kengine/helpers/type_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename... Comps>
	void register_attributes(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		putils::for_each_type<Comps...>([&](const auto & t) {
			KENGINE_PROFILING_SCOPE;

			using type = putils_wrapped_type(t);
			if constexpr (putils::reflection::has_class_name<type>())
				kengine_logf(r, log, "Init/register_meta_components", "Registering attributes for %s", putils::reflection::get_class_name<type>());

			const auto type_entity = type_helper::get_type_entity<type>(r);
			r.emplace<meta::attributes>(type_entity, &putils::reflection::runtime::get_attributes<type>());
		});
	}
}
