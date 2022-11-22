# [registerCopy](registerCopy.hpp)

```cpp
template<typename ... Comps>
void registerCopy() noexcept;
```

Registers a standard implementation of the [Copy](../../components/meta/Copy.md) `meta Component` for each type in `Comps`.