# [RegisterComponentJSONLoader](RegisterComponentJSONLoader.hpp)

Helper functions to register sample implementations of the [LoadFromJSON](../components/meta/LoadFromJSON.md) `meta Component`.

## Members

### registerComponentJSONLoader

```cpp
template<typename Comp>
void registerComponentJSONLoader(EntityManager & em);
```

Implements the `LoadFromJSON` `meta Component` for `Comp`.

### registerComponentJSONLoaders

```cpp
template<typename ... Comps>
void registerComponentJSONLoaders(EntityManager & em);
```

Calls `registerComponentJSONLoader<T>` for each `T` in `Comps`.