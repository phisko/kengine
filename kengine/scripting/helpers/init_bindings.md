# [init_bindings](init_bindings.hpp)

```cpp
template<typename Func, typename Func2>
void init_bindings(entt::registry & r, Func && register_function, Func2 && register_type) noexcept;
```

Registers global entity manipulation functions with a scripting language. Calls `register_type` with `putils::meta::type<Entity>` as parameter.