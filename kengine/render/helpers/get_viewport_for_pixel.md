# [get_viewport_for_pixel](get_viewport_for_pixel.hpp)

```cpp
struct viewport_info {
    entt::entity camera = entt::null;
    putils::point2f pixel = { -1.f, 1.f }; // [0,1]
};

viewport_info get_viewport_for_pixel(entt::handle window_entity, const putils::point2ui & pixel) noexcept;
```

Returns the [camera](../data/camera.md) which is displayed in a given pixel of a given [window](../data/window.md).

In addition to the camera, the function also returns the pixel's position as a percentage of the camera's viewport.