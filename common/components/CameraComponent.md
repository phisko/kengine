# [CameraComponent](CameraComponent.hpp)

`Component` that lets `Entities` be used as in-game cameras.

Its overall layout is very similar to that of the [TransformComponent](TransformComponent.md), head over there if you need to learn about the template parameters and type specializations.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

##### frustrum

```cpp
putils::Rect<Precision, Dimensions> frustrum;
```

Specifies the frustrum for the camera, i.e. its on-screen rendering coordinates and size.

##### pitch, yaw

```cpp
Precision pitch; // Radians
Precision yaw; // Radians
```