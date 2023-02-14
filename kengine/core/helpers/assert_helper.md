# [assert_helper](assert_helper.hpp)

Helper macros and functions providing higher-level `assert` functionality.

## Members

If `KENGINE_NDEBUG` is defined, all the following macros have no effect.

### kengine_assert(r, condition)

Checks that `condition` is true. If not, breaks the debugger on the current line and calls `assert_helper::assert_failed(__FILE__, __LINE__, "condition")`.

### kengine_assert_with_message(r, condition, message)

Same behavior as `kengine_assert`, except the message passed to `assert_helper::assert_failed` is `message`.

### kengine_assert_failed(r, message)

Same behavior as `kengine_assert_with_message`, but with no condition to check. Always breaks the debugger and calls `assert_helper::assert_failed`.

### assert_helper namespace

#### assert_failed

```cpp
void assert_failed(const entt::registry & r, const char * file, int line, const char * message) noexcept;
```

Logs the callstack. Calls the `assert_handler`, if it is set.

#### is_debugger_present

```cpp
bool is_debugger_present() noexcept;
```

Returns whether a debugger is attached.
