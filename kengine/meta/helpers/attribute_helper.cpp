#include "attribute_helper.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/runtime_helper.hpp"

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/helpers/assert_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine meta
#include "kengine/meta/helpers/type_helper.hpp"

namespace kengine::meta::attribute_helper {
	const putils::reflection::runtime::attribute_info * find_attribute(entt::handle type_entity, std::string_view path, std::string_view separator) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*type_entity.registry(), very_verbose, "attribute_helper", "Finding [%u]'s '%s' attribute ", type_entity.entity(), std::string(path).c_str());

		const auto attrs = type_entity.try_get<attributes>();
		if (!attrs) {
			const auto name_comp = type_entity.try_get<data::name>();
			const auto type_name = name_comp ? name_comp->name.c_str() : "<unknown>";
			kengine_assert_failed(*type_entity.registry(), "Cannot search attributes for '", type_name, "' without meta::attributes");
			return nullptr;
		}

		return putils::reflection::runtime::find_attribute(*attrs->type_attributes, path, separator);
	}
}
