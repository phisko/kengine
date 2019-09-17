# [TransformComponent](TransformComponent.hpp)

`Component` providing an `Entity`'s position and size. Template parameters are the type used for the coordinates (`Precision`) and the number of `Dimensions` (2 or 3).

Some specializations are pre-defined:

```cpp
using TransformComponent2i = TransformComponent<int, 2>;
using TransformComponent3i = TransformComponent<int, 3>;

using TransformComponent2d = TransformComponent<double, 2>;
using TransformComponent3d = TransformComponent<double, 3>;

using TransformComponent2f = TransformComponent<float, 2>;
using TransformComponent3f = TransformComponent<float, 3>;
```

All the `Systems` provided with the *kengine* use `TransformComponent3f`.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

### Members

##### Constructor

```cpp
TransformComponent(const putils::Point<Precision, Dimensions> &pos = { 0, 0, 0 },
                   const putils::Point<Precision, Dimensions> &size = { 1, 1, 1 });
```

##### boundingBox

```cpp
putils::Rect<Precision, Dimensions> boundingBox;
```
The `Entity`'s position is stored as `boundingBox.position`, and its size as `boundingBox.size`.

##### pitch, yaw, roll

```cpp
Precision pitch = 0; // Radians
Precision yaw = 0; // Radians
Precision roll = 0; // Radians
```
