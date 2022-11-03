#include "attributeHelper.hpp"

// putils
#include "reflection_helpers/runtime_helper.hpp"

// kengine data
#include "data/NameComponent.hpp"

// kengine helpers
#include "helpers/typeHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::meta::attributeHelper {
	const putils::reflection::runtime::AttributeInfo * findAttribute(const Entity & typeEntity, std::string_view path, std::string_view separator) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto nameComp = typeEntity.tryGet<NameComponent>();
		const auto typeName = nameComp ? nameComp->name.c_str() : "<unknown>";

		const auto attributes = typeEntity.tryGet<Attributes>();
		if (!attributes) {
			kengine_assert_failed("Cannot search attributes for '", typeName, "' without meta::Attributes");
			return nullptr;
		}

		return putils::reflection::runtime::findAttribute(*attributes->attributes, path, separator);
	}
}
