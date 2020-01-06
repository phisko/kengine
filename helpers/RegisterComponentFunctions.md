# [RegisterComponentFunctions](RegisterComponentFunctions.hpp)

Helper functions to register sample implementations of the [Has](../components/meta/Has.md), [AttachTo](../components/meta/AttachTo.md) and [DetachFrom](../components/meta/DetachFrom.md) `meta Components`.

## Members

### registerComponentFunctions

```cpp
template<typename Comp>
void registerComponentFunctions(EntityManager & em);
```

Implements the `Has`, `AttachTo` and `DetachFrom` `meta Components` for `Comp`.

### registerComponentsFunctions

```cpp
template<typename ... Comps>
void registerComponentsFunctions(EntityManager & em);
```

Calls `registerComponentFunctions<T>` for each `T` in `Comps`.