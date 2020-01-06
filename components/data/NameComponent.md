# [NameComponent](NameComponent.hpp)

`Component` providing the name of an `Entity`.

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Serializable (POD)

## Members

### name

```cpp
putils::string<KENGINE_NAME_COMPONENT_MAX_LENGTH> name;
```

The maximum length of this field (stored as a [putils::string](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_NAME_COMPONENT_MAX_LENGTH` macro.