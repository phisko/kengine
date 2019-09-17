# [ModelComponent](ModelComponent.hpp)

`Component` providing graphical information about a "model" file (be it a 3D model, a 2D sprite or any other form of renderable element).

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

##### file

```cpp
putils::string<KENGINE_MODEL_STRING_MAX_LENGTH> file;
```

The model file this component describes.

##### boundingBox

```cpp
putils::Rect3f boundingBox = { {}, { 1, 1, 1 } };
```

Defines the offset and scale to be applied to the model.

##### pitch, yaw, roll

```cpp
float pitch = 0.f; // Radians
float yaw = 0.f; // Radians
float roll = 0.f; // Radians
```

##### mirrored

```cpp
bool mirrored = false;
```
