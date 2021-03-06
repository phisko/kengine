# [GetEntityInPixel](GetEntityInPixel.hpp)

`Function Component` that lets users query the ID of the `Entity` seen in a given pixel.

This `function Component` is typically attached to graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)), as they are the ones able to implement it according to their rendering technique.

## Prototype

```cpp
Entity::ID (Entity::ID window, const putils::Point2ui & pixel);
```

### Return value

* `Entity::INVALID_ID` if the `window` is unknown to this handler (i.e. it is handled by another graphics system)
* `Entity::INVALID_ID` if no `Entity` is seen in the pixel
* the ID of the `Entity` seen in the pixel otherwise

### Parameters

* `window`: ID of the ["window" Entity](WindowComponent.md) we are inspecting
* `pixel`: pixel coordinates we are querying

## Usage

Here is a small example that creates an input handler which, when the user clicks their mouse, outputs the ID of the `Entity` seen in the clicked pixel.

This piece of code is very similar to the behavior of the [OnClickSystem](../../systems/OnClickSystem.md).

```cpp
EntityManager em;

em += [](Entity & e) {
    InputComponent input;
    input.onMouseButton = [&](Entity::ID window, int button, const putils::Point2f & pixel, bool pressed) {
        for (const auto & [e, getEntity] : em.getEntities<functions::GetEntityInPixel>()) {
            const auto id = getEntity(window, pixel);
            std::cout << id << '\n';
        }
    };

    e += input;
};
```