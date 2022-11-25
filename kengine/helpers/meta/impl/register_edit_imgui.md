# [register_edit_imgui](register_edit_imgui.hpp)

```cpp
template<typename ... Comps>
void register_edit_imgui(entt::registry & r) noexcept;
```

Registers a standard implementation of the [edit_imgui](../../../meta/edit_imgui.md) `meta component` for each type in `Comps`.