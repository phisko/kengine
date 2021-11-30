#include "attributeHelper.hpp"

#include "data/NameComponent.hpp"

#ifndef KENGINE_ATTRIBUTE_MAX_PATH
# define KENGINE_ATTRIBUTE_MAX_PATH 1024
#endif

namespace kengine::meta::attributeHelper {
	const Attributes::AttributeInfo * findAttribute(const Entity & typeEntity, std::string_view path, std::string_view separator) noexcept {
		const auto nameComp = typeEntity.tryGet<NameComponent>();
		const auto typeName = nameComp ? nameComp->name.c_str() : "<unknown>";

		const auto attributes = typeEntity.tryGet<Attributes>();
		if (!attributes) {
			kengine_assert_failed("Cannot search attributes for '", typeName, "' without meta::Attributes");
			return nullptr;
		}

		const Attributes::AttributeMap * currentAttributes = &attributes->attributes;
		putils::string<KENGINE_ATTRIBUTE_MAX_PATH> currentPath(path);

		auto dotPos = currentPath.find(separator);
		while (dotPos != std::string::npos) {
			const auto nextAttribute = currentPath.substr(0, dotPos);
			currentPath = currentPath.substr(dotPos + separator.length());
			dotPos = currentPath.find(separator);

			const auto it = currentAttributes->find(nextAttribute.c_str());
			if (it == currentAttributes->end()) {
				kengine_assert_failed("Unknown attribute '", path, "' in '", typeName, "'");
				return nullptr;
			}

			currentAttributes = &it->second.attributes;
		}

		const auto it = currentAttributes->find(currentPath.c_str());
		if (it == currentAttributes->end()) {
			kengine_assert_failed("Unknown attribute '", path, "' in '", typeName, "'");
			return nullptr;
		}

		return &it->second;
	}
}
