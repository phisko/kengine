# [viewport](viewport.hpp)

Component providing display information for ["camera" entities](camera.md).

## Members

### window

```cpp
entt::entity window = entt::null;
```

[Window entity](window.md) to which this camera renders. If this is not set explicitly by the user, the first graphics system to be processed will set it to that system's window.

### bounding_box

```cpp
putils::rect2f bounding_box = { { 0.f, 0.f }, { 1.f, 1.f } };
```

On-screen bounding box (in screen percentage) where the camera should be displayed.

### z_order

```cpp
float z_order = 1.f;
```

Cameras with a higher Z order appear "above" cameras with a lower Z order.

### resolution

```cpp
putils::point2i resolution = { 1280, 720 };
```

Rendering resolution for the camera. This is independant of the final display resolution, meaning you can easily upscale or downscale (i.e. render in 1280x720 but display in 1920x1080).

### render_texture

```cpp
using render_texture = void *;
render_texture render_texture = INVALID_RENDER_TEXTURE;
```

ID for a texture to which the camera is rendered (in addition to being displayed on-screen). The type of this field depends on the graphics systems.