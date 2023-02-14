# [name](name.hpp)

Component that names the entity.

## Members

### name

```cpp
putils::string<KENGINE_NAME_STRING_MAX_LENGTH> name;
```

The maximum length of this field (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 64, and can be adjusted by defining the `KENGINE_NAME_STRING_MAX_LENGTH` macro.