# [OnClickComponent](OnClickComponent.hpp)

`Component` that defines a callback for when an `Entity` is clicked.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (contains a functor)
* Processed by the [OnClickSystem](../../systems/onclick/OnClickSystem.md)

## Members

### onClick

```cpp
putils::function<void(), KENGINE_ONCLICK_FUNCTION_SIZE> onClick = nullptr;
```

Callback for when the `Entity` is clicked.

The maximum size for the callback defaults to 64 and can be adjusted by defining the `KENGINE_ONCLICK_FUNCTION_SIZE` macro.