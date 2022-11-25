# [sky_box](sky_box.hpp)

Component that draws a skybox.

## Members

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

### color

```cpp
putils::normalized_color color;
```

color filter to be applied over the skybox. Stored as a [putils::normalized_color](https://github.com/phisko/putils/blob/master/putils/color.md).