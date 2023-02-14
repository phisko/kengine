# [sky_box](sky_box.hpp)

Component that draws a skybox. The entity's [model](model.md) must have a `sky_box_model` component.

## Members

### color

```cpp
putils::normalized_color color;
```

Color filter to be applied over the skybox. Stored as a [putils::normalized_color](https://github.com/phisko/putils/blob/master/putils/color.md).

# [sky_box_model](sky_box.hpp)

[Model component](model.md) that lists the textures required to draw a skybox.

### textures

```cpp
using string = putils::string<KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME>;

string right;
string left;
string top;
string bottom;
string front;
string back;
```

The maximum length of a texture name (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 128, and can be adjusted by defining the `KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME` macro.