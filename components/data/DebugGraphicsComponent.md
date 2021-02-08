 # [DebugGraphicsComponent](DebugGraphicsComponent.hpp)

`Component` that provides debug information to be drawn for an `Entity`.

The position and scale of the element that will be drawn is relative to the `Entity`'s [TransformComponent](TransformComponent.md).

Debug information can be:
* text
* line
* sphere
* box

The maximum length of the debug text and font (stored as [putils::strings](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_DEBUG_GRAPHICS_TEXT_MAX_LENGTH` macro.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### Element types

```cpp
struct Text {
    std::string text;
    std::string font;
    float size = 1.f;
};

struct Line {
    putils::Point3f end = {};
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

    Text text;
    Line line;
    Sphere sphere;
    Box box;

    Type type;
};

std::vector<Element> elements;
```

Collection of elements to be drawn for this entity.