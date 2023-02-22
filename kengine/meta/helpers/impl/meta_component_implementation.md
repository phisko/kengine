# [meta_component_implementation](meta_component_implementation.hpp)

```cpp
template<typename Meta, typename T>
struct meta_component_implementation : std::true_type {};
```

Class template providing the implementation of the `Meta` meta component for `T`.

Specializations must define a static `function` function with the same prototype as the meta component.
They must also expose a static `value` boolean, used to enable or disable the specialization for a given type.

For examples, see [the engine's meta component implementations](./).