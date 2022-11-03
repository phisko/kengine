#pragma once

// putils
#include "reflection_helpers/runtime_helper.hpp"

// kengine meta
#include "meta/Attributes.hpp"

// kengine helpers
#include "helpers/typeHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
    template<typename ... Comps>
    void registerAttributes() noexcept {
		KENGINE_PROFILING_SCOPE;
        putils::for_each_type<Comps...>([](const auto & t) {
			KENGINE_PROFILING_SCOPE;

            using Type = putils_wrapped_type(t);
			if constexpr (putils::reflection::has_class_name<Type>())
				kengine_logf(Log, "Init/registerMetaComponents", "Registering Attributes for %s", putils::reflection::get_class_name<Type>());

        	auto type = typeHelper::getTypeEntity<Type>();
        	type += meta::Attributes{ &putils::reflection::runtime::getAttributes<Type>() };
        });
    }
}
