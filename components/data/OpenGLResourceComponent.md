# [OpenGLResourceComponent](OpenGLResourceComponent.hpp)

`Components` holding RAII handles to OpenGL resources.

## DepthMapComponent

### Members

#### fbo

```cpp
putils::gl::FrameBuffer fbo;
```

Framebuffer object used for drawing into the shadow map.

#### texture

```cpp
putils::gl::Texture texture;
```

Texture into which the shadow map will be drawn.

#### size

```cpp
int size = -1;
```

Size of the shadow map.

## CSMComponent

Holds a cascaded shadow map.

## DepthCubeComponent

Holds a "shadow cube".