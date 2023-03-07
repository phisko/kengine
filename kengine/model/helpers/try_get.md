# [try_get](try_get.hpp)

```cpp
template<typename Comp>
const Comp * try_get(entt::const_handle instance) noexcept;

template<typename Comp>
const Comp * try_get(const entt::registry & r, const model::instance & instance) noexcept;
```

Returns the `Comp` component of the `model entity` for `instance`, or `nullptr` if there is none.
