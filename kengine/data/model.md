# [model](model.hpp)

Component that represents a resource file.

Entities with this component are referred to as `model entities`. They hold [flyweight](https://www.wikiwand.com/en/Flyweight_pattern)-style data about all entities that are [instances of the model](instance.md).

## Members

### file

```cpp
putils::string<KENGINE_MODEL_STRING_MAX_LENGTH> file;
```

The model file this component describes.

The maximum length of a filename (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 64, and can be adjusted by defining the `KENGINE_MODEL_STRING_MAX_LENGTH` macro.