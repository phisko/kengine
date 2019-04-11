# [BehaviorComponent](BehaviorComponent.hpp)

`Component` providing a function to execute for a `GameObject`.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Members

##### func

```cpp
std::function<void()> func = nullptr;
```

Function to execute.
