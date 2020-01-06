# [InitGBuffer](InitGBuffer.hpp)

`Function Component` that provides the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md) with the information it needs to initialize the GBuffer, as well as an "iterator function" so that tools can list the various GBuffer textures.

## Prototype

```cpp
void (size_t nbAttributes, const GBufferAttributeIterator & iterator);
```

### Parameters

* `nbAttributes`: number of GBuffer textures
* `iterator`: function that takes a `void(const char *)` functor as parameter and calls it for each GBuffer texture, passing it the name as parameter

## Usage

This `function Component` is mostly for internal use of the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md), which calls it automatically with a default set of GBuffer textures if none are provided by the user.

A helper `initGBuffer` function is provided which takes a structure defining the GBuffer textures as template parameter and automatically calls the `function Component`.