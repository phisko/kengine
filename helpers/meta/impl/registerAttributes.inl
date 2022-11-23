#pragma once

// entt
#include <entt/entity/registry.hpp>

// putils
#include "reflection_helpers/runtime_helper.hpp"

// kengine meta
#include "meta/Attributes.hpp"

// kengine helpers
#include "helpers/typeHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
    template<typename ... Comps>
    void registerAttributes(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
        putils::for_each_type<Comps...>([&](const auto & t) {
			KENGINE_PROFILING_SCOPE;

            using Type = putils_wrapped_type(t);
			if constexpr (putils::reflection::has_class_name<Type>())
				kengine_logf(r, Log, "Init/registerMetaComponents", "Registering Attributes for %s", putils::reflection::get_class_name<Type>());

        	const auto type = typeHelper::getTypeEntity<Type>(r);
			r.emplace<meta::Attributes>(type, &putils::reflection::runtime::getAttributes<Type>());
        });
    }
}
