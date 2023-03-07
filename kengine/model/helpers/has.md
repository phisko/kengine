# [has](has.hpp)

```cpp
template<typename Comp>
bool has(entt::const_handle instance);

template<typename Comp>
bool has(const entt::registry & r, const model::instance & instance);
```

Returns whether the `model entity` for `instance` has a `Comp` component.