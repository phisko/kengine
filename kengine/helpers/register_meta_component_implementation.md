# [register_meta_component_implementation](register_meta_component_implementation.hpp)

```cpp
template<typename Meta, typename ... Comps, typename Func>
void register_meta_component_implementation(entt::registry & r, Func && f) noexcept;
```

Registers an implementation of the `Meta` `meta component` for each component type in `Comps`.

## Parameters

* `Meta`: type of the `meta component` for which the implementation will be registered
* `Comps...`: set of component types for which to implement `Meta`
* `f`: implementation that will be registered for `Meta`. Its prototype should be that of `Meta`, with an additional `putils::meta::type<T>` as its first parameter (used to identify the parent component type from within the implementation)

## Predicate version

```cpp
template<typename Meta, template<typename T> typename Predicate, typename ... Comps, typename Func>
void register_meta_component_implementation_with_predicate(entt::registry & r, Func && f) noexcept;
```

Will only register the implementation if `Predicate<Component>` is true.