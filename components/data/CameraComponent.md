# [CameraComponent](CameraComponent.hpp)

`Component` that lets `Entities` be used as in-game cameras.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### frustum

```cpp
putils::Rect3f frustum;
```

Specifies the frustum for the camera, i.e. its world coordinates and "size" (FOV for 3D, actual size for 2D).

### pitch, yaw, roll

```cpp
float pitch; // Radians
float yaw; // Radians
float roll; // Radians
```

### nearPlane, farPlane

```cpp
float nearPlane = 1.f;
float farPlane = 1000.f;
```