# [WindowComponent](WindowComponent.hpp)

`Component` that lets `Entities` be used as operating system windows.

"Window `Entities`" are automatically created by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md)) if no user-created "window `Entity`" is available.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/opengl/OpenGLSystem.md))

## Members

### name

```cpp
using string = putils::string<KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH>;
string name;
```

Name of the window.

The maximum length of this field (stored as a [putils::string](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_WINDOW_COMOPNENT_NAME_MAX_LENGTH` macro.

### size

```cpp
putils::Point2ui size = { 1280, 720 };
```

### fullscreen

```cpp
bool fullscreen = false;
```

### shutdownOnClose

```cpp
bool shutdownOnClose = true;
```

Indicates whether the application should shut down when users close the window.

### assignedSystem

```cpp
string assignedSystem;
```

Name of the graphics system handling this window.