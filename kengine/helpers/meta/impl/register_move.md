# [register_move](register_move.hpp)

```cpp
template<typename ... Comps>
void register_move(entt::registry & r) noexcept;
```

Registers a standard implementation of the [move](../../../meta/move.md) `meta component` for each type in `Comps`.