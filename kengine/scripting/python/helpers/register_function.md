# [register_function](register_function.hpp)

```cpp
template<typename F>
void register_function(const char * name, F && func) noexcept;
```

Register a new function with the Python state.