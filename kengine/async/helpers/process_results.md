# [process_results](process_results.hpp)

```cpp
template<typename T>
bool process_results(entt::registry & r) noexcept;

template<typename T, std::invocable<entt::entity, T &&> Func>
bool process_results(entt::registry & r, Func && func) noexcept;
```

Checks all running async tasks returning `T`. Each completed task is removed, and its result is passed to `func`.

The function returns `true` if all tasks returning `T` are complete.