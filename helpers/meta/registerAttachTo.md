# [registerAttachTo](registerAttachTo.hpp)

```cpp
template<typename ... Comps>
void registerAttachTo() noexcept;
```

Registers a standard implementation of the [AttachTo](../../components/meta/AttachTo.md) `meta Component` for each type in `Comps`.