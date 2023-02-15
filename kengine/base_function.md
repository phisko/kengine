# [base_function](base_function.hpp)

```cpp
template<typename Ret, typename ... Args>
struct base_function<Ret(Args...)>;
```

Base type for `function components` (see [README](../README.md#function-components) for more information about these). Function components can simply inherit from `base_function`, templated on the function signature.

## Members

### func

```cpp
putils::function<Ret(Args...), KENGINE_FUNCTION_MAX_SIZE> func = nullptr;
```

Functor to be called. Its maximum size defaults to 64, and can be changed by defining `KENGINE_FUNCTION_MAX_SIZE`.

### call

```cpp
Ret call(Args ... args) const noexcept;
Ret operator()(Args... args) const noexcept;
```

Calls the functor.