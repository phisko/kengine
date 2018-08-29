# [BehaviorComponent](BehaviorComponent.hpp)

`Component` providing a function to execute for a `GameObject`.

Inherits from [putils::Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md).

### Members

##### func

```cpp
std::function<void()> func = nullptr;
```

Function to execute for `GameObject`.
