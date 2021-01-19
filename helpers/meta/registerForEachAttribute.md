# [registerForEachAttribute](registerForEachAttribute.hpp)

```cpp
template<typename ... Comps>
void registerForEachAttribute() noexcept;
```

Registers a standard implementation of the [ForEachAttribute](../../components/meta/ForEachAttribute.md) `meta Component` for each type in `Comps`.