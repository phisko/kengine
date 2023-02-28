# [set_context](set_context.hpp)

```cpp
bool set_context(const entt::registry & r) noexcept;
```

Finds an existing [ImGui context](../data/context.md) and sets it as current. This is useful in multi-DLL contexts, where ImGui is statically linked and therefore doesn't share its global context across DLL boundaries.