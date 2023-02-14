# [window](window.hpp)

Component that creates an operating system window.

"Window entities" are automatically created by graphics systems if no user-created window entity is available.

## Members

### name

```cpp
using string = putils::string<KENGINE_WINDOW_COMPONENT_NAME_MAX_LENGTH>;
string name;
```

Name of the window.

The maximum length of this field (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 64, and can be adjusted by defining the `KENGINE_WINDOW_COMOPNENT_NAME_MAX_LENGTH` macro.

### size

```cpp
putils::point2ui size = { 1280, 720 };
```

### fullscreen

```cpp
bool fullscreen = false;
```

### assigned_system

```cpp
string assigned_system;
```

Name of the graphics system handling this window.