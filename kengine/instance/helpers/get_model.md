# [get_model](get_model.hpp)

```cpp
template<typename Comp>
const Comp & get_model(entt::const_handle instance);

template<typename Comp>
const Comp & get_model(const entt::registry & r, const instance::instance & instance);
```

Returns the `Comp` component of the `model entity` for `instance`.