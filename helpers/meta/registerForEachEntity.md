# [registerForEachEntity](registerForEachEntity.hpp)

```cpp
template<typename ... Comps>
void registerForEachEntity() noexcept;
```

Registers a standard implementation of the [ForEachEntity](../../components/meta/ForEachEntity.md) and `ForEachEntityWithout` `meta Components` for each type in `Comps`.