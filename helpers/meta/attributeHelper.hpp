#pragma once

#include "kengine.hpp"
#include "meta/Attributes.hpp"

namespace kengine::meta::attributeHelper {
	const putils::reflection::AttributeInfo * findAttribute(const Entity & typeEntity, std::string_view path, std::string_view separator = ".") noexcept;
}