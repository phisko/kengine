#include "attributeHelper.hpp"

// kengine
#include "data/NameComponent.hpp"
#include "helpers/typeHelper.hpp"

// putils
#include "reflection_helpers/runtime_helper.hpp"

namespace kengine::meta::attributeHelper {
	const putils::reflection::runtime::AttributeInfo * findAttribute(const Entity & typeEntity, std::string_view path, std::string_view separator) noexcept {
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
