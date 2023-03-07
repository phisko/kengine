# [model_has](model_has.hpp)

```cpp
template<typename Comp>
bool model_has(entt::const_handle instance);

template<typename Comp>
bool model_has(const entt::registry & r, const instance::instance & instance);
```

Returns whether the `model entity` for `instance` has a `Comp` component.