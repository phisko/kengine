# [assert_failed](assert_failed.hpp)

```cpp
bool assert_failed(const entt::registry & r, const char * file, int line, const char * expr) noexcept;
```

Logs an error and notifies [assert handlers](../functions/on_assert_failed.md).