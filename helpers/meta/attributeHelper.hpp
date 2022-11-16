#pragma once

#include "kengine.hpp"

// kengine meta
#include "meta/Attributes.hpp"

namespace kengine::meta::attributeHelper {
	KENGINE_CORE_EXPORT const putils::reflection::runtime::AttributeInfo * findAttribute(const Entity & typeEntity, std::string_view path, std::string_view separator = ".") noexcept;
}