# TransformComponent

`Component` providing a `GameObject`'s position and size. Template parameters are the type used for the coordinates (`Precision`) and the number of `Dimensions` (2 or 3).

Inherits from [putils::Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md).

Some specializations are pre-defined:

```
using TransformComponent2i = TransformComponent<int, 2>;
using TransformComponent3i = TransformComponent<int, 3>;

using TransformComponent2d = TransformComponent<double, 2>;
using TransformComponent3d = TransformComponent<double, 3>;

using TransformComponent2f = TransformComponent<float, 2>;
using TransformComponent3f = TransformComponent<float, 3>;
```

### Members

##### Constructor

```
TransformComponent(const putils::Point<Precision, 3> &pos = { 0, 0, 0 },
                   const putils::Point<Precision, 3> &size = { 1, 1, 1 });
```

##### boundingBox

```
putils::Rect<Precision, Dimensions> boundingBox;
```
The `GameObject`'s position is stored as `boundingBox.topLeft`, and its size as `boundingBox.size`.

##### yaw

```
Precision yaw = 0; // Radians
```
