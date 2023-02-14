# [register_meta_component_implementation](register_meta_component_implementation.hpp)

```cpp
template<typename Meta, typename ... Comps>
void register_meta_component_implementation(entt::registry & r) noexcept;
```

Registers an implementation of the `Meta` `meta component` for each component type in `Comps`.

## Parameters

* `Meta`: type of the `meta component` for which the implementation will be registered
* `Comps...`: set of component types for which to implement `Meta`

## Behavior

Meta component implementations are defined by specializing this class template:

```cpp
template<typename Meta, typename T>
struct meta_component_implementation : std::true_type {};
```

The specialization must define a static `function` function with the same prototype as the meta component.
It must also expose a static `value` boolean, used to enable or disable the specialization for a given type.

For examples, see [the engine's meta component implementations](meta/impl/).