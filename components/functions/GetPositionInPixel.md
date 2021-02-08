# [GetPositionInPixel](GetPositionInPixel.hpp)

`Function Component` that lets users query the position of the `Entity` seen in a given pixel. The returned position is the precise position seen in the pixel, not the general position of the `Entity`.

This `function Component` is typically attached to graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)), as they are the ones able to implement it according to their rendering technique.

## Prototype

```cpp
std::optional<putils::Point3f> (Entity::ID window, const putils::Point2ui & pixel);
```

### Return value

* `std::nullopt` if the `window` is unknown to this handler (i.e. it is handled by another graphics system)
* `std::nullopt` if no `Entity` is seen in the pixel
* the position seen in the pixel otherwise

### Parameters

* `window`: ID of the ["window" Entity](WindowComponent.md) we are inspecting
* `pixel`: pixel coordinates we are querying

## Usage

Here is a small example that creates an input handler which, when the user clicks their mouse, outputs the ID of the `Entity` seen in the clicked pixel.

This piece of code is very similar to the behavior of the [OnClickSystem](../../systems/onclick/OnClickSystem.md).

```cpp
entities += [](Entity & e) {
    InputComponent input;
    input.onMouseButton = [](Entity::ID window, int button, const putils::Point2f & pixel, bool pressed) {
        for (const auto & [e, getPosition] : entities.with<functions::GetPositionInPixel>()) {
            const auto pos = getPosition(window, pixel);
            std::cout << pos.x << ' ' << pos.y << ' ' << pos.z << '\n';
        }
    };

    e += input;
};
```