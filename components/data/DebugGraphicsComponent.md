 # [DebugGraphicsComponent](DebugGraphicsComponent.hpp)

`Component` that provides debug information to be drawn for an `Entity`.

The position and scale of the element that will be drawn is relative to the `Entity`'s [TransformComponent](TransformComponent.md).

Debug information can be:
* line
* sphere
* box

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable
* Processed by graphics systems

## Members

### Element types

```cpp
struct Line {
    putils::Point3f end = { 0.f, 0.f, 0.f };
    float thickness = 1.f;
};

struct Sphere {
    float radius = .5f;
};

struct Box {
    putils::Vector3f size = { 1.f, 1.f, 1.f };
};
```

These are the representations for the various type of debug elements.

### Reference space

```cpp
enum class ReferenceSpace {
    World,
    Object
};
```

Specifies whether an element's position is specified in world-space or object-space.

### Elements

```cpp
struct Element {
    putils::Point3f pos{ 0.f, 0.f, 0.f };
    putils::NormalizedColor color;

    ReferenceSpace referenceSpace = ReferenceSpace::Object;

    Line line;
    Sphere sphere;
    Box box;

    Type type;
};

std::vector<Element> elements;
```

Collection of elements to be drawn for this entity.