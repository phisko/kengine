# [ComponentSettings](ComponentSettings.hpp)

Component-specific settings that let users control how a specific `Component` will be handled by the engine.

## map

```cpp
template<typename T>
using map = std::unordered_map<EntityID, T, std::hash<EntityID>, std::equal_to<EntityID>, allocator<T>>;
```

Defines what type should be used internally to map an `EntityID` to a `Component`.

I've chosen to use `std::unordered_map` as the default map, mainly for the benefit of pointers and references to elements never being invalidated. This lets you access `Components` from multiple threads, without having to worry about your reference being invalidated by an insertion in some other thread. If you know that a given `Component` type will only be accessed from single-threaded, or "safe" (i.e. insertion-free) multiple threads, you may consider using something like Google's `flat_hash_map` instead.

## allocator

```cpp
template<typename T>
using allocator = putils::MemoryPool<std::pair<const EntityID, T>, memoryPoolBlockSize<T>>;
```

If the default `map` type is used, defines what allocator should be used for it.

I've chosen to replace `std::allocator` with a modified version of [MemoryPool](https://github.com/cacay/MemoryPool). This lets `Components` of a given type be grouped together and allows for better cache coherency.

## memoryPoolBlockSize

```cpp
template<typename T>
constexpr size_t memoryPoolBlockSize = 4096;
```

If the default `map` and `allocator` are used, defines the size that will be allocated by the `MemoryPool` for each block.