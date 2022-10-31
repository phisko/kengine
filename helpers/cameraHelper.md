# [cameraHelper](cameraHelper.hpp)

Helper functions for manipulating [camera Entities](../components/data/CameraComponent.md).

## Members

### getViewportForPixel

```cpp
struct ViewportInfo {
    EntityID camera = INVALID_ID;
    putils::Point2f pixel = { -1.f, 1.f }; // [0,1]
};

ViewportInfo getViewportForPixel(EntityID windowID, const putils::Point2ui & pixel) noexcept;
```

Returns the ID of the [camera Entity](../components/data/CameraComponent.md) which is displayed in a given pixel of a given [window](../components/data/WindowComponent.md).

In addition to the camera's ID, the function also returns the pixel's position as a percentage of the camera's viewport.

### entityAppearsInViewport

```cpp
bool entityAppearsInViewport(const Entity & entity, const Entity & viewport) noexcept;
```

Returns whether `entity` should appear in `viewport`.

### convertToScreenPercentage

```cpp
putils::Rect3f convertToScreenPercentage(const putils::Rect3f & rect, const putils::Point2f & screenSize, const OnScreenComponent & comp) noexcept;
```

Depending on the `coordinateType` of `comp`, convertts `rect` to the correct screen percentage.

### getFacings

```cpp
struct Facings {
    putils::Vector3f front;
    putils::Vector3f right;
    putils::Vector3f up;
};

Facings getFacings(const CameraComponent & camera) noexcept;
```

Returns the facing vectors for `camera`.