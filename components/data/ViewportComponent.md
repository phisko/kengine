# [ViewportComponent](ViewportComponent.hpp)

`Component` providing display information for ["camera" Entities](CameraComponent.md).

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)

## Members

### window

```cpp
EntityID window = INVALID_ID;
```

["Window" Entity](WindowComponent.md) in which this camera should be rendered. If this is not set explicitly by the user, the first graphics system to be processed will set it to the window `Entity` they created.

### boundingBox

```cpp
putils::Rect2f boundingBox = { { 0.f, 0.f }, { 1.f, 1.f } };
```

On-screen bounding box (in screen percentage) where the camera should be displayed.

### zOrder

```cpp
float zOrder = 1.f;
```

Cameras with a higher Z order will be displayed "above" cameras with a lower Z order.

### resolution

```cpp
putils::Point2i resolution = { 1280, 720 };
```

Rendering resolution for the camera. This is independant of the final display resolution, meaning you can easily upscale or downscale (i.e. render in 1280x720 but display in 1920x1080).

### renderTexture

```cpp
using RenderTexture = void *;
RenderTexture renderTexture = INVALID_RENDER_TEXTURE;
```

ID for a texture to which the camera is rendered (in addition to being displayed on-screen). The type of this field depends on the graphics systems (for instance, it's a `GLuint` for the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)).