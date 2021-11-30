# [TransformComponent](TransformComponent.hpp)

`Component` providing an `Entity`'s position and size. 

Some specializations are pre-defined:

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)), physics systems (such as the [BulletSystem](../../systems/bullet/BulletSystem.md))...

## Members

### boundingBox

```cpp
putils::Rect3f boundingBox{ { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } };
```

The `Entity`'s position is stored as `boundingBox.position`, and its scale as `boundingBox.size`.

### pitch, yaw, roll

```cpp
float pitch = 0; // Radians
float yaw = 0; // Radians
float roll = 0; // Radians
```