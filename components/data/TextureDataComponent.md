# [TextureDataComponent](TextureDataComponent.hpp)

`Component` holding texture data, that can be used by graphics systems to load the texture onto the GPU.

## Specs

* Not [reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Not serializable (contains pointers and OpenGL identifiers)
* Initialized by model loading systems (such as the [AssImpSystem](../../systems/assimp/AssImpSystem.md)) and processed by graphics systems, such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md).

## Usage

Systems wishing to load and use a texture should create a ["model" Entity](ModelComponent.md) for the texture and attach a `TextureDataComponent` to it, having:
* populated the `data`, `width`, `height` and `components` fields with the texture data
* initialized the `free` field with the function pointer needed to free the texture data, if necessary

Graphics systems will then load the texture onto the GPU and attach a [SystemSpecificTextureComponent](SystemSpecificTextureComponent.md).

## Members

### data

```cpp
void * data;
```

Pointer to the raw texture data that should be loaded into OpenGL.

### width, height, components

```cpp
int width;
int height;
int components;
```

Size information about `data`.

### free

```cpp
using FreeFunc = void(*)(void * data);
FreeFunc free;
```

Pointer to a function that will be called to release `data` once the texture has been loaded into OpenGL.