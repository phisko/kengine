# [registerLoadFromJSON](registerLoadFromJSON.hpp)

```cpp
template<typename ... Comps>
void registerLoadFromJSON() noexcept;
```

Registers a standard implementation of the [LoadFromJSON](../../components/meta/LoadFromJSON.md) `meta Component` for each type in `Comps`.