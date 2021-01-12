# [ShaderComponent](ShaderComponent.hpp)

```cpp
template<typename T>
struct SystemSpecificShaderComponent;
```

`Component` that lets `Entities` be used as custom shaders. `T` is a system-specific type ([putils::gl::Program](../../putils/opengl/Program.md) for the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md), for instance).

Shaders can either be:
* GBuffer shaders: used to fill the deferred rendering GBuffer
* Lighting shaders: reading the GBuffer and applying lighting to fill the screen
* "Post-lighting" shaders: applying lighting effects which require the first lighting pass to have already been performed (such as godrays, which need shadow maps to have been populated already)
* Post-process shaders
* Shadow map shaders: used to fill a shadow map for a given light
* Shadow cube shaders: used to fill a shadow cube for a given light

Tag `Components` are provided to mark the type of a shader.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (contains pointers)
* Processed by the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)

## Members

### shader

```cpp
std::unique_ptr<T> shader;
```

Pointer to the system-specific shader.

### enabled

```cpp
bool enabled = true;
```

Lets users enable or disable shaders at runtime.