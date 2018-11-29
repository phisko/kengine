# [LuaComponent](LuaComponent.hpp)

`Component` that specifies the list of lua scripts to be run for the `GameObject` it is attached to.

Scripts can use the `self` global variable to access the `GameObject` they are attached to.

Inherits from [putils::Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md).

In all the following functions, a `script` is the path to a file containing a script, NOT THE ACTUAL SCRIPT CODE.

### Members

##### Constructor

```cpp
LuaComponent(const std::vector<std::string> &scripts = {});
```

##### attachScript

```cpp
void attachScript(std::string_view file) noexcept;
```

##### removeScript

```cpp
void removeScript(std::string_view file) noexcept;
```

##### getScripts

```cpp
const std::vector<std::string> &getScripts() const noexcept;
```

##### meta

```cpp
sol::table meta;
```

Lua scripts can set this value to anything they desire.
