#include "attributeHelper.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "reflection_helpers/runtime_helper.hpp"

// kengine data
#include "data/NameComponent.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/typeHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::meta::attributeHelper {
	const putils::reflection::runtime::AttributeInfo * findAttribute(entt::handle typeEntity, std::string_view path, std::string_view separator) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto attributes = typeEntity.try_get<Attributes>();
		if (!attributes) {
			const auto nameComp = typeEntity.try_get<NameComponent>();
			const auto typeName = nameComp ? nameComp->name.c_str() : "<unknown>";
			kengine_assert_failed(*typeEntity.registry(), "Cannot search attributes for '", typeName, "' without meta::Attributes");
			return nullptr;
		}

		return putils::reflection::runtime::findAttribute(*attributes->attributes, path, separator);
	}
}
