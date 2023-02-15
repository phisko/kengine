 # [debug_graphics](debug_graphics.hpp)

Component that draws debug elements.

The position and scale of the element that will be drawn is relative to the entity's [transform](../../core/data/transform.md).

Debug information can be:
* line
* sphere
* box

## Members

### Element types

```cpp
struct line_element {
    putils::point3f end = { 0.f, 0.f, 0.f };
    float thickness = 1.f;
};

struct sphere_element {
    float radius = .5f;
};

struct box_element {
    putils::vec3f size = { 1.f, 1.f, 1.f };
};
```

These are the representations for the various type of debug elements.

### Reference space

```cpp
enum class reference_space {
    world,
    object
};
```

Specifies whether an element's position is specified in world-space or object-space.

### Elements

```cpp
struct element {
    putils::point3f pos{ 0.f, 0.f, 0.f };
    putils::normalized_color color;

    reference_space reference_space = reference_space::object;

    line_element line;
    sphere_element sphere;
    box_element box;

    element_type type;
};

std::vector<element> elements;
```

Collection of elements to be drawn for this entity.