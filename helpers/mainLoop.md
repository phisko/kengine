# [mainLoop](mainLoop.hpp)

Helper functions to use as an application's main loop.

## Members

### run

```cpp
void run(EntityManager & em);
```

As long as `em.running` is `true`, loops over all `Entities` with an [Execute](../components/functions/Execute.md) `function Component` and calls them with the calculated delta time.
