# [register_storage](register_storage.hpp)

```cpp
template<typename... Comps>
void register_storage(entt::registry & r) noexcept;
```

Pre-instantiates the storage for `Comps` in `r`.

This avoids possible race conditions if:
* a worker thread (e.g. a loading thread) is constantly creating and accessing components
* the main thread executes the system, which makes use of a new component type for the first time, thus creating its storage

## is_storage_registered

```cpp
template<typename... Comps>
bool is_storage_registered(const entt::registry & r) noexcept;
```

Returns whether the storage for `Comps` exists.