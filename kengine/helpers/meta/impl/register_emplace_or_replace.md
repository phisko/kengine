# [register_emplace_or_replace](register_emplace_or_replace.hpp)

```cpp
template<typename ... Comps>
void register_emplace_or_replace(entt::registry & r) noexcept;
```

Registers a standard implementation of the [emplace_or_replace](../../../meta/emplace_or_replace.md) `meta component` for each type in `Comps`.