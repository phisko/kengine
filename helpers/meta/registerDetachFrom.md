# [registerDetachFrom](registerDetachFrom.hpp)

```cpp
template<typename ... Comps>
void registerDetachFrom() noexcept;
```

Registers a standard implementation of the [DetachFrom](../../components/meta/DetachFrom.md) `meta Component` for each type in `Comps`.