# [is_running](is_running.hpp)

Helper functions to query and manipulate the engine's state. These are based on [keep_alive components](../data/keep_alive.md).

## Members

### is_running

```cpp
bool is_running(const entt::registry & r) noexcept;
```

Returns `true` if at least one `keep_alive` exists.

### stop_running

```cpp
KENGINE_CORE_EXPORT void stop_running(entt::registry & r) noexcept;
```

Removes all `keep_alive` components.