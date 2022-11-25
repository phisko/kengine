# [register_display_imgui](register_display_imgui.hpp)

```cpp
template<typename ... Comps>
void register_display_imgui(entt::registry & r) noexcept;
```

Registers a standard implementation of the [display_imgui](../../../meta/display_imgui.md) `meta component` for each type in `Comps`.