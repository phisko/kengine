# [LuaComponent](LuaComponent.hpp)

`Component` that specifies the list of lua scripts to be run for the `GameObject` it is attached to.

Scripts can use the `self` global variable to access the `GameObject` they are attached to.

```
/!\ Calling `createEntity` or `removeEntity` from a script attached to a `GameObject` will result in the entity being created or remove **only at the end of the current frame**. This is necessary to avoid memory corruption errors, as the newly created entities could invalidate iterators in the collection of entities being currently iterated on by the [LuaSystem](../systems/LuaSystem.md).
```

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
sol::object meta;
```

Lua scripts can set this value to anything they desire.
