#pragma once

#include "kengine.hpp"
#include "helpers/typeHelper.hpp"
#include "meta/Attributes.hpp"

namespace kengine::meta::attributeHelper {
	template<typename T>
	const Attributes::AttributeInfo * findAttribute(std::string_view path, std::string_view separator = ".") noexcept {
		return findAttribute(typeHelper::getTypeEntity<T>(), path, separator);
	}
	
	const Attributes::AttributeInfo * findAttribute(const Entity & typeEntity, std::string_view path, std::string_view separator = ".") noexcept;
}