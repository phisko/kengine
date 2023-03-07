#include "find_attribute.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/runtime_helper.hpp"

// kengine
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/meta/helpers/get_type_entity.hpp"

namespace kengine::meta {
	const putils::reflection::runtime::attribute_info * find_attribute(entt::handle type_entity, std::string_view path, std::string_view separator) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*type_entity.registry(), very_verbose, "meta", "Finding {}'s '{}' attribute ", type_entity, path);

		const auto attrs = type_entity.try_get<attributes>();
		if (!attrs) {
			const auto name_comp = type_entity.try_get<core::name>();
			const auto type_name = name_comp ? name_comp->name.c_str() : "<unknown>";
			kengine_assert_failed(*type_entity.registry(), "Cannot search attributes for '{}' without meta::attributes", type_name);
			return nullptr;
		}

		return putils::reflection::runtime::find_attribute(*attrs->type_attributes, path, separator);
	}
}
