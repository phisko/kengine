#pragma once

// stl
#include <optional>
#include <unordered_map>

// kengine impl
#include "impl/ID.hpp"

// #define KENGINE_DEBUG_PRINT_COMPONENT_ID

namespace kengine::componentSettings {
    template<typename T>
    using allocator = std::allocator<std::pair<const EntityID, std::optional<T>>>;

	template<typename T>
	using map = std::unordered_map<EntityID, std::optional<T>, std::hash<EntityID>, std::equal_to<EntityID>, allocator<T>>;
}

#define KENGINE_COMPONENT_MEMORY_POOL_BLOCK_SIZE(T, size)\
	 template<>\
	 constexpr size_t kengine::componentSettings::memoryPoolBlockSize<T> = size;

#define KENGINE_COMPONENT_ALLOCATOR(T, alloc)\
	 template<>\
	 using kengine::componentSettings::allocator<T> = alloc;

#define KENGINE_COMPONENT_MAP(T, m)\
	 template<>\
	 using kengine::componentSettings::map<T> = m;
