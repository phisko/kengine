# [register_match_string](register_match_string.hpp)

```cpp
template<typename ... Comps>
void register_match_string(entt::registry & r) noexcept;
```

Registers a standard implementation of the [match_string](../../../meta/match_string.md) `meta component` for each type in `Comps`.