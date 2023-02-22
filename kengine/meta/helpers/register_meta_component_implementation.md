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

Meta component implementations are defined by specializing [meta_component_implementation](impl/meta_component_implementation.md).