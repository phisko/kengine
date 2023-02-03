# [backward_compatible_observer](backward_compatible_observer.hpp)

```cpp
template<typename... Comps>
struct backward_compatible_observer;
```

Wrapper for `entt::observer` that also processes entities created before it. It currently only observes entities entering a group.

## Members

### Nested types

```cpp
using callback_signature = void(entt::entity, Comps & ...);
using callback_type = std::function<callback_signature>;
```

This is the signature of the callback that will be held and called for each entity that passes the filter.

### Constructor

```cpp
backward_compatible_observer(entt::registry & r, const callback_type & callback) noexcept;
```

* Registers the observer with `r`
* Calls `callback` for each entity with `Comps`
* Copies `callback` so that it may be called in future calls to `process`

### Process

```cpp
void process() noexcept;
```

Iterates through the wrapped `ent::observer` and calls `callback` for each of those entities.