# [registerComponentAttributeIterators](registerComponentAttributeIterators.hpp)

Helper functions to register sample implementations of the [ForEachAttribute](../components/meta/ForEachAttribute.md) `meta Component`.

## Members

### registerComponentAttributeIterators

```cpp
template<typename Comp>
void registerComponentAttributeIterator(EntityManager & em);
```

Implements the `ForEachAttribute` `meta Component` for `Comp`.

### registerComponentAttributeIterators

```cpp
template<typename ... Comps>
void registerComponentAttributeIterators(EntityManager & em);
```

Calls `registerComponentAttributeIterator<T>` for each `T` in `Comps`.
