# [CameraComponent](CameraComponent.hpp)

`Component` that lets `Entities` be used as in-game cameras.

Its overall layout is very similar to that of the [TransformComponent](TransformComponent.md), head over there if you need to learn about the template parameters and type specializations.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

##### frustum

```cpp
putils::Rect<Precision, Dimensions> frustum;
```

Specifies the frustum for the camera, i.e. its on-screen rendering coordinates and size.

##### pitch, yaw, roll

```cpp
Precision pitch; // Radians
Precision yaw; // Radians
Precision roll; // Radians
```
