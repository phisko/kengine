# [run](run.hpp)

Helper functions to use as an application's main loop.

## Members

### run

```cpp
void run(entt::registry & r) noexcept;
```

As long as [is_running](is_running.md) returns `true`, loops over all entities with an [execute](../functions/execute.md) `function component` and calls them with the calculated delta time.

### time_modulated::run

```cpp
namespace time_modulated {
    void run(entt::registry & r) noexcept;
}
```

Does the same as `run`, but modulates the delta time according to any [time_modulator components](../data/time_modulator.md).