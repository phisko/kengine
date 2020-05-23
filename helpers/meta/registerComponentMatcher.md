# [registerComponentMatcher](registerComponentMatcher.hpp)

Helper functions to register sample implementations of the [MatchString](../components/meta/MatchString.md) `meta Component`.

## Members

### registerComponentMatcher

```cpp
template<typename Comp>
void registerComponentMatcher(EntityManager & em);
```

Implements the `MatchString` `meta Component` for `Comp`.

### registerComponentMatchers

```cpp
template<typename ... Comps>
void registerComponentMatchers(EntityManager & em);
```

Calls `registerComponentMatcher<T>` for each `T` in `Comps`.
