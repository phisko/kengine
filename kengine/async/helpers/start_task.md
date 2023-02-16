# [start_task](start_task.hpp)

Helper functions for running [async tasks](../data/task.md).

## Members

### start_task

```cpp
template<typename T>
void start_task(entt::registry & r, entt::entity e, const task::string & task_name, std::future<T> && future) noexcept;
```

Runs an async task returning `T`. The function will attach to `e`:
* a [task](../data/task.md) initialized with `task_name`
* an internal structure containing `future`

`future` may have been created however the user wishes (typically by calling `std::async`).