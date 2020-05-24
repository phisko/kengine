# [instanceHelper](instanceHelper.hpp)

Helper functions to manipulate [instance and model Entities](../components/data/InstanceComponent.md).

## Members

### modelHas

```cpp
template<typename Comp>
bool modelHas(EntityManager & em, const Entity & instance);
template<typename Comp>
bool modelHas(EntityManager & em, const InstanceComponent & instance);
```

Returns whether the `model Entity` for `instance` has a `Comp` component.

### getModel

```cpp
template<typename Comp>
Comp & getModel(EntityManager & em, Entity & instance);
template<typename Comp>
Comp & getModel(EntityManager & em, const InstanceComponent & instance);
```

Returns the `Comp` component of the `model Entity` for instance.