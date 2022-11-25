# [instance_helper](instance_helper.hpp)

Helper functions to manipulate [instance and model entities](../data/instance.md).

## Members

### model_has

```cpp
template<typename Comp>
bool model_has(entt::const_handle instance);
template<typename Comp>
bool model_has(const entt::registry & r, const data::instance & instance);
```

Returns whether the `model entity` for `instance` has a `Comp` component.

### get_model

```cpp
template<typename Comp>
const Comp & get_model(entt::const_handle instance);
template<typename Comp>
const Comp & get_model(const entt::registry & r, const data::instance & instance);
```

Returns the `Comp` component of the `model entity` for `instance`.

### try_get_model

```cpp
template<typename Comp>
const Comp * try_get_model(entt::const_handle instance) noexcept;
template<typename Comp>
const Comp * try_get_model(const entt::registry & r, const data::instance & instance) noexcept;
```

Returns the `Comp` component of the `model entity` for `instance`, or `nullptr` if there is none.