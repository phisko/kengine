# [new_entity_processor](new_entity_processor.hpp)

```cpp
template<typename ProcessedTag, typename... Comps>
struct new_entity_processor;
```

Helper type to automatically iterate over new entities matching a group of components. Iterated entities are marked with `ProcessedTag`, ensuring that they'll only be processed once.

## Members

### Nested types

```cpp
using callback_signature = void(entt::entity e, Comps &... comps); // or void(entt::entity) if any of the types in Comps is empty
using callback_type = std::function<callback_signature>;
```

### Constructor

```cpp
new_entity_processor(entt::registry & r, const callback_type & callback) noexcept;
```

`callback` will be called during each call to `process`. The constructor will check if `ProcessedTag`'s storage was pre-registered and emit a (verbose) log message if not. This can help diagnose race conditions due to storage creation conflicts.