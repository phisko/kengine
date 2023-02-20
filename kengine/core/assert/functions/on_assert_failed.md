# [on_assert_failed](on_assert_failed.hpp)

`Function component` called when an assertion fails.

## Prototype

```cpp
bool (const char * file, int line, const std::string & expr);
```

### Parameters

* `file`: the file where the assertion was emitted
* `line`: the line where the assertion was emitted
* `expr`: the assertion expression, or a custom message if one was provided

### Return value

Returns whether the debugger should break. If multiple `on_assert_failed` are found, any one of them returning `true` will break the debugger.