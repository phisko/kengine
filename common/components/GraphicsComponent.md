# [GraphicsComponent](GraphicsComponent.hpp)

`Component` providing graphical information about an `Entity`.

Inherits from [putils::Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md).

### Members

##### Layer

Represents one of the possibly many layers used to draw an `Entity`. A `Layer` stores:

* a `name`, for indexing
* an `appearance`, the path to the texture to be drawn
* a `boundingBox`, the relative position and size compared to the `Entity`'s `TransformComponent`
* a `yaw`
* a `mirrored` boolean

##### Constructor

```cpp
GraphicsComponent(const std::string & appearance = "");
```

Adds a *"main"* layer with the specified appearance.

##### addLayer

```cpp
Layer & addLayer(const std::string & name, const std::string & appearance);
```

##### removeLayer

```cpp
void removeLayer(const std::string & name);
```

##### hasLayer

```cpp
bool hasLayer(const std::string & name);
```

##### getLayer

```cpp
Layer & getLayer(const std::string & name);
```
