# [TextureModelComponent](TextureModelComponent.hpp)

`Component` that marks an `Entity` as a ["model" Entity](ModelComponent.md) for an OpenGL texture. This is used internally by the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md) and its shaders.

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Not serializable (contains OpenGL identifiers)
* Processed by the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)

## Members

### file

```cpp
putils::string<KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH> file;
```

Texture file represented by this `Component`.

The maximum length of the filename (stored as a [putils::string](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_TEXTURE_MODEL_COMPONENT_PATH_MAX_LENGTH` macro.

### texture

```cpp
GLuint texture = -1;
```

OpenGL texture ID for the texture.