# [GraphicsComponent](GraphicsComponent.hpp)

`Component` providing graphical information about an `Entity`. This will be used to find its [model Entity](ModelComponent.md).

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD), although the [model](#model) attribute will be invalidated
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### appearance

```cpp
putils::string<KENGINE_GRAPHICS_STRING_MAX_LENGTH, stringName> appearance;
```

Path to the texture or 3D model (or whatever the rendering system might need) to be drawn for the `Entity`.

The maximum length of the appearance (stored as a [putils::string](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_GRAPHICS_STRING_MAX_LENGTH` macro.

### color

```cpp
putils::NormalizedColor color = { 1.f, 1.f, 1.f, 1.f };
```

Color filter to be applied to the `Entity`. Stored as a [putils::NormalizedColor](https://github.com/phisko/putils/blob/master/Color.md).