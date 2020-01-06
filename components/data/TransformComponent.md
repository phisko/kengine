# [TransformComponent](TransformComponent.hpp)

`Component` providing an `Entity`'s position and size. 

Some specializations are pre-defined:

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)), physics systems (such as the [BulletSystem](../../systems/bullet/BulletSystem.md))...

## Members

### Constructor

```cpp
TransformComponent(const putils::Point3f &pos = { 0, 0, 0 },
                   const putils::Point3f &size = { 1, 1, 1 });
```

### boundingBox

```cpp
putils::Rect3f boundingBox;
```

The `Entity`'s position is stored as `boundingBox.position`, and its scale as `boundingBox.size`.

### pitch, yaw, roll

```cpp
Precision pitch = 0; // Radians
Precision yaw = 0; // Radians
Precision roll = 0; // Radians
```