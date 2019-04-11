# [GraphicsComponent](GraphicsComponent.hpp)

`Component` providing graphical information about an `Entity`.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

##### Layer

Represents one of the possibly many layers used to draw an `Entity`. A `Layer` stores:

* a `name`, for indexing
* an `appearance`, the path to the texture (or 3D model or whatever the rendering `System` might need) to be drawn
* a `boundingBox`, the relative position and size compared to the `Entity`'s `TransformComponent`
* a `pitch`
* a `yaw`
* a `mirrored` boolean

The maximum length of the layer's name and appearance (stored as [putils::strings](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_GRAPHICS_STRING_MAX_LENGTH` macro.

The maximum number of layers defaults to 8 and can be adjusted by defining the `KENGINE_GRAPHICS_MAX_LAYERS` macro.

##### Constructor

```cpp
GraphicsComponent(const char * appearance = "");
```

Adds a *"main"* layer with the specified appearance.

##### addLayer

```cpp
Layer & addLayer(const char * name, const char * appearance);
```

##### removeLayer

```cpp
void removeLayer(const char * name);
```

##### hasLayer

```cpp
bool hasLayer(const char * name);
```

##### getLayer

```cpp
Layer & getLayer(const char * name);
```
