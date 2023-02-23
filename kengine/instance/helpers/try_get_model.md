# [try_get_model](try_get_model.hpp)

```cpp
template<typename Comp>
const Comp * try_get_model(entt::const_handle instance) noexcept;

template<typename Comp>
const Comp * try_get_model(const entt::registry & r, const instance::instance & instance) noexcept;
```

Returns the `Comp` component of the `model entity` for `instance`, or `nullptr` if there is none.
