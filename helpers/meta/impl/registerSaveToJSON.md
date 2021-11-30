# [registerSaveToJSON](registerSaveToJSON.hpp)

```cpp
template<typename ... Comps>
void registerSaveToJSON() noexcept;
```

Registers a standard implementation of the [SaveToJSON](../../components/meta/SaveToJSON.md) `meta Component` for each type in `Comps`.