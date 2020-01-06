# [RegisterComponentEntityIterators](RegisterComponentEntityIterators.hpp)

Helper functions to register sample implementations of the [ForEachEntity and ForEachEntityWithout](../components/meta/ForEachEntity.md) `meta Components`.

## Members

### registerComponentEntityIterators

```cpp
template<typename Comp>
void registerComponentEntityIterator(EntityManager & em);
```

Implements the `ForEachEntity` and `ForEachEntityWithout` `meta Components` for `Comp`.

### registerComponentEntityIterators

```cpp
template<typename ... Comps>
void registerComponentEntityIterators(EntityManager & em);
```

Calls `registerComponentEntityIterator<T>` for each `T` in `Comps`.