# [ModelComponent](ModelComponent.hpp)

`Component` providing graphical information about a "model" file (be it a 3D model, a 2D sprite or any other form of renderable element).

`Entities` with this `Component` are sometimes referred to as "model Entities". They hold [flyweight](https://www.wikiwand.com/en/Flyweight_pattern)-style data about all `Entities` using the model.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Initialized by model-loading systems (such as the [AssImpSystem](../../systems/assimp/AssImpSystem.md)) and then used by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)) (and others).

## Members

### file

```cpp
putils::string<KENGINE_MODEL_STRING_MAX_LENGTH> file;
```

The model file this component describes.

The maximum length of a filename (stored as a [putils::string](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_MODEL_STRING_MAX_LENGTH` macro.

### boundingBox

```cpp
putils::Rect3f boundingBox = { {}, { 1, 1, 1 } };
```

Defines the offset and scale to be applied to the model.

### pitch, yaw, roll

```cpp
float pitch = 0.f; // Radians
float yaw = 0.f; // Radians
float roll = 0.f; // Radians
```

### mirrored

```cpp
bool mirrored = false;
```
