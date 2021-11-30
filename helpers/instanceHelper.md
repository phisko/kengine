# [instanceHelper](instanceHelper.hpp)

Helper functions to manipulate [instance and model Entities](../components/data/InstanceComponent.md).

## Members

### modelHas

```cpp
template<typename Comp>
bool modelHas(const Entity & instance);
template<typename Comp>
bool modelHas(const InstanceComponent & instance);
```

Returns whether the `model Entity` for `instance` has a `Comp` component.

### getModel

```cpp
template<typename Comp>
const Comp & getModel(const Entity & instance);
template<typename Comp>
const Comp & getModel(const InstanceComponent & instance);
```

Returns the `Comp` component of the `model Entity` for `instance`.

### tryGetModel

```cpp
template<typename Comp>
const Comp * tryGetModel(const Entity & instance) noexcept;
template<typename Comp>
const Comp * tryGetModel(const InstanceComponent & instance) noexcept;
```

Returns the `Comp` component of the `model Entity` for `instance`, or `nullptr` if there is none.