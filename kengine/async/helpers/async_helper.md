# [async_helper](async_helper.hpp)

Helper functions for running [async tasks](../data/async_task.md).

## Members

### start_async_task

```cpp
template<typename T>
void start_async_task(entt::registry & r, entt::entity e, const data::async_task::string & task_name, std::future<T> && future) noexcept;
```

Runs an async task returning `T`. The function will attach to `e`:
* an [async_task](../data/async_task.md) initialized with `task_name`
* an internal structure containing `future`

`future` may have been created however the user wishes (typically by calling `std::async`).

### process_async_results

```cpp
template<typename T>
bool process_async_results(entt::registry & r) noexcept;

template<typename T, std::invocable<entt::entity, T &&> Func>
bool process_async_results(entt::registry & r, Func && func) noexcept;
```

Checks all running async tasks returning `T`. Each completed task, is removed, and its result is passed to `func`.

The function returns `true` if all tasks returning `T` are complete.