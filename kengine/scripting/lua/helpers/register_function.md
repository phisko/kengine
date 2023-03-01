# [register_function](register_function.hpp)

```cpp
template<typename F>
void register_function(const entt::registry & r, const char * name, F && func) noexcept;
```

Register a new function with the Lua state.