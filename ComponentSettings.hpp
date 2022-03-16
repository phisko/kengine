#pragma once

#include <unordered_map>
#include "MemoryPool.hpp"

#include "impl/ID.hpp"

// #define KENGINE_DEBUG_PRINT_COMPONENT_ID

namespace kengine::componentSettings {
	template<typename T>
	constexpr size_t memoryPoolBlockSize = 4096;

	template<typename T>
	using allocator = putils::MemoryPool<std::pair<const EntityID, T>, memoryPoolBlockSize<T>>;

	template<typename T>
	using map = std::unordered_map<EntityID, T, std::hash<EntityID>, std::equal_to<EntityID>, allocator<T>>;
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
