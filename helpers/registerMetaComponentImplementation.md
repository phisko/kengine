# [registerMetaComponentImplementation](registerMetaComponentImplementation.hpp)

```cpp
template<typename Meta, typename ... Comps, typename Func>
void registerMetaComponentImplementation(Func && f) noexcept;
```

Registers an implementation of the `Meta` `meta Component` for each `Component` type in `Comps`.

## Parameters

* `Meta`: type of the `meta Component` for which the implementation will be registered
* `Comps...`: set of `Component` types for which to implement `Meta`
* `f`: implementation that will be registered for `Meta`. Its prototype should be that of `Meta`, with an additional `putils::meta::type<T>` as its first parameter (used to identify the parent `Component` type from within the implementation)