# [SkyBoxComponent](SkyBoxComponent.hpp)

`Component` that lets an `Entity` be used as a skybox for graphics systems.

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

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

The maximum length of a texture name (stored as a [putils::string](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 128, and can be adjusted by defining the `KENGINE_SKYBOX_TEXTURE_PATH_MAX_NAME` macro.

### color

```cpp
putils::NormalizedColor color;
```

Color filter to be applied over the skybox. Stored as a [putils::NormalizedColor](https://github.com/phiste/putils/blob/master/Color.md).