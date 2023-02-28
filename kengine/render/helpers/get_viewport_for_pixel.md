# [camera_helper](camera_helper.hpp)

Helper functions for manipulating [cameras](../data/camera.md).

## Members

### get_viewport_for_pixel

```cpp
struct viewport_info {
    entt::entity camera = entt::null;
    putils::point2f pixel = { -1.f, 1.f }; // [0,1]
};

viewport_info get_viewport_for_pixel(entt::handle window_entity, const putils::point2ui & pixel) noexcept;
```

Returns the [camera](../data/camera.md) which is displayed in a given pixel of a given [window](../data/window.md).

In addition to the camera, the function also returns the pixel's position as a percentage of the camera's viewport.

### entity_appears_in_viewport

```cpp
bool entity_appears_in_viewport(const entt::registry & r, entt::entity entity, entt::entity viewport) noexcept;
```

Returns whether `entity` should appear in `viewport`.

### convert_to_screen_percentage

```cpp
putils::rect3f convert_to_screen_percentage(const putils::rect3f & rect, const putils::point2f & screen_size, const data::on_screen & comp) noexcept;
```

Depending on the `coordinates` of `comp`, converts `rect` to the correct screen percentage.

### get_facings

```cpp
struct facings {
    putils::vec3f front;
    putils::vec3f right;
    putils::vec3f up;
};

facings get_facings(const data::camera & camera) noexcept;
```

Returns the facing vectors for `camera`.