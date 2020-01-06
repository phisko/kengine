# [CameraHelper](CameraHelper.hpp)

Helper functions for manipulating [camera Entities](../components/data/CameraComponent.md).

## Members

### getViewportForPixel

```cpp
struct ViewportInfo {
    Entity::ID camera = Entity::INVALID_ID;
    putils::Point2f pixel = { -1.f, 1.f }; // [0,1]
};

ViewportInfo getViewportForPixel(EntityManager & em, Entity::ID windowID, const putils::Point2ui & pixel);
```

Returns the ID of the [camera Entity](../components/data/CameraComponent.md) which is displayed in a given pixel of a given [window](../components/data/WindowComponent.md).

In addition to the camera's ID, the function also returns the pixel's position as a percentage of the camera's viewport.