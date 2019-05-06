# [BehaviorComponent](BehaviorComponent.hpp)

`Component` providing a function to execute for an `Entity`.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Members

##### func

```cpp
putils::function<void(), KENGINE_BEHAVIOR_FUNCTION_SIZE> func = nullptr;
```

Function to execute. Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_BEHAVIOR_FUNCTION_SIZE` macro.