# [imgui_lua_helper](imgui_lua_helper.hpp)

Helper function to quickly initialize ImGui bindings for Lua.

## Members

### init_bindings

```cpp
void init_bindings(const entt::registry & r) noexcept;
```

Initializes the bindings. Should be called after having added the [lua system](../systems/lua/lua.md).