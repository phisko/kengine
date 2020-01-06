# [GraphicsComponent](GraphicsComponent.hpp)

`Component` providing graphical information about an `Entity`.

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Serializable (POD), although the [model](#model) attribute will be invalidated
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### appearance

```cpp
putils::string<KENGINE_GRAPHICS_STRING_MAX_LENGTH, stringName> appearance;
```

Path to the texture or 3D model (or whatever the rendering system might need) to be drawn for the `Entity`.

The maximum length of the appearance (stored as a [putils::string](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_GRAPHICS_STRING_MAX_LENGTH` macro.

### color

```cpp
putils::NormalizedColor color = { 1.f, 1.f, 1.f, 1.f };
```

Color filter to be applied to the `Entity`. Stored as a [putils::NormalizedColor](https://github.com/phiste/putils/blob/master/Color.md).

### model

```cpp
Entity::ID model = Entity::INVALID_ID;
```

Unless you are performing some sort of optimization, this field can be safely ignored. It will be automatically filled by the graphics systems if it is not set.

Points to a `model` entity, which holds a [ModelComponent](ModelComponent.md) with information about the texture or 3D model (or whatever the rendering system might need).

If this is unclear, here's a little diagram to help:

```
[ Player entity ]          
GraphicsComponent: appearance = "player", model = 42

[ Model entity ]
id = 42
ModelComponent: appearance = "player", yaw = 3.14 // example model-specific transformation
```