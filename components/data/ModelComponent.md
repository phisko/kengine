# [ModelComponent](ModelComponent.hpp)

`Component` indicating that an `Entity` represents a resource file.

`Entities` with this `Component` are sometimes referred to as "model Entities". They hold [flyweight](https://www.wikiwand.com/en/Flyweight_pattern)-style data about all `Entities` [using the model](InstanceComponent.md).

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)

## Members

### file

```cpp
putils::string<KENGINE_MODEL_STRING_MAX_LENGTH> file;
```

The model file this component describes.

The maximum length of a filename (stored as a [putils::string](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_MODEL_STRING_MAX_LENGTH` macro.