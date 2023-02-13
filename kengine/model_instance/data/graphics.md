# [graphics](graphics.hpp)

Component providing graphical information about the entity. This will be used to find its [model entity](model.md).

## Members

### appearance

```cpp
putils::string<KENGINE_GRAPHICS_STRING_MAX_LENGTH, string_name> appearance;
```

Path to the texture or 3D model (or whatever the rendering system might need) to be drawn for the entity.

The maximum length of the appearance (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_GRAPHICS_STRING_MAX_LENGTH` macro.

### color

```cpp
putils::normalized_color color = { 1.f, 1.f, 1.f, 1.f };
```

Color filter to be applied to the entity. Stored as a [putils::normalized_color](https://github.com/phisko/putils/blob/master/putils/color.md).