# [registerComponentFunctions](registerComponentFunctions.hpp)

```cpp
template<typename ... Comps>
void registerComponentsFunctions() noexcept;
```

Registers sample implementations of the [Has](../components/meta/Has.md), [AttachTo](../components/meta/AttachTo.md) and [DetachFrom](../components/meta/DetachFrom.md) `meta Components` for `Comps`.