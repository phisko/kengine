# [TextureDataComponent](TextureDataComponent.hpp)

`Component` used to load a texture into OpenGL and obtain a `texture ID`.

## Specs

* Not [reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Not serializable (contains pointers and OpenGL identifiers)
* Initialized by model loading systems (such as the [AssImpSystem](../../systems/assimp/AssImpSystem.md)) and processed by the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md).

## Usage

Systems wishing to obtain an OpenGL `texture ID` should create a ["model" Entity](ModelComponent.md) for the texture and attach a `TextureDataComponent` to it, having:
* populated the `data`, `width`, `height` and `components` fields with the texture data
* initialized the `textureID` field to point to the `GLuint` where you'd like to store the texture ID
* initialized the `free` field with the function pointer needed to free the texture data

During the next frame, the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md) will load the texture into OpenGL and initialize the texture ID.

## Members

### data

```cpp
void * data;
```

Pointer to the raw texture data that should be loaded into OpenGL.

### textureID

```cpp
GLuint * textureID;
```

Pointer to the OpenGL `texture ID` that the texture will be stored in. This is an "out parameter": the `texture ID` will be initialized and filled by the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md).

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