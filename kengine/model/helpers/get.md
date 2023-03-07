# [get](get.hpp)

```cpp
template<typename Comp>
const Comp & get(entt::const_handle instance);

template<typename Comp>
const Comp & get(const entt::registry & r, const model::instance & instance);
```

Returns the `Comp` component of the `model entity` for `instance`.