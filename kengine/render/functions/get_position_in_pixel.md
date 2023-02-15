# [get_position_in_pixel](get_position_in_pixel.hpp)

`Function component` that returns the position seen in a given pixel.

Typically attached to graphics systems, as they are the ones able to implement it according to their rendering technique.

## Prototype

```cpp
std::optional<putils::point3f> (entt::entity window, const putils::point2ui & pixel);
```

### Return value

* `std::nullopt` if the `window` is unknown to this handler (i.e. it is handled by another graphics system)
* `std::nullopt` if nothing is seen in the pixel
* the position seen in the pixel otherwise

### Parameters

* `window`: [window](../data/window.md) we are inspecting
* `pixel`: pixel coordinates we are querying

## Usage

Here is a small example that creates an input handler which, when the user clicks their mouse, outputs the position seen in the clicked pixel.

This piece of code is very similar to the behavior of the [on_click system](../../on_click/systems/on_click.md).

```cpp
const auto e = r.create();
data::input input;
input.on_mouse_button = [](entt::entity window, int button, const putils::point2f & pixel, bool pressed) {
    for (const auto & [e, get_position] : r.view<functions::get_position_in_pixel>().each()) {
        const auto pos = get_position(window, pixel);
        std::cout << pos.x << ' ' << pos.y << ' ' << pos.z << std::endl;
    }
};
r.emplace<data::input>(e, std::move(input));
```