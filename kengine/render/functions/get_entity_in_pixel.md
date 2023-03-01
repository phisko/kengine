# [get_entity_in_pixel](get_entity_in_pixel.hpp)

`Function component` that returns the entity seen in a given pixel.

This `function component` is typically attached to graphics systems, as they are the ones able to implement it according to their rendering technique.

## Prototype

```cpp
entt::entity (entt::entity window, const putils::point2ui & pixel);
```

### Return value

* `entt::null` if the `window` is unknown to this handler (i.e. it is handled by another graphics system)
* `entt::null` if no entity is seen in the pixel
* the entity seen in the pixel otherwise

### Parameters

* `window`: [window](../data/window.md) we are inspecting
* `pixel`: pixel coordinates we are querying

## Usage

Here is a small example that creates an input handler which, when the user clicks their mouse, outputs the ID of the entity seen in the clicked pixel.

This piece of code is very similar to the behavior of the [on_click system](../on_click/systems/system.md).

```cpp
const auto e = r.create();
input::handler input;
input.on_mouse_button = [](entt::entity window, int button, const putils::point2f & pixel, bool pressed) {
    for (const auto & [e, get_entity] : r.view<functions::get_entity_in_pixel>().each()) {
        const auto id = get_entity(window, pixel);
        std::cout << id << std::endl;
    }
};
r.emplace<input::handler>(e, std::move(input));
```