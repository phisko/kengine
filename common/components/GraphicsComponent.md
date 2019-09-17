# [GraphicsComponent](GraphicsComponent.hpp)

`Component` providing graphical information about an `Entity`.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

* an `appearance`, the path to the texture (or 3D model or whatever the rendering `System` might need) to be drawn
* the `Entity::ID` pointing to a `model` entity, which holds a [ModelComponent](ModelComponent.md) with information about a model.
* a `color` filter

The `model` field will automatically be filled by the graphics systems if it is not set.

If this is unclear, here's a little diagram to help:

```
[ Player entity ]          
GraphicsComponent: appearance = "player", model = 42

[ Model entity ]
id = 42
ModelComponent: appearance = "player", yaw = 3.14
```

The maximum length of the layer's name and appearance (stored as [putils::strings](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_GRAPHICS_STRING_MAX_LENGTH` macro.
