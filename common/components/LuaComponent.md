# [LuaComponent](LuaComponent.hpp)

`Component` that specifies the list of lua scripts to be run for the `Entity` it is attached to.

Scripts can use the `self` global variable to access the `Entity` they are attached to.

In all the following functions, a `script` is the path to a file containing a script, NOT THE ACTUAL SCRIPT CODE.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

The maximum length of a script name (stored as a [putils::string](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_MAX_LUA_SCRIPT_PATH` macro.

The maximum number of scripts defaults to 8 and can be adjusted by defining the `KENGINE_MAX_LUA_SCRIPTS` macro.

### Members

##### Types

```cpp
using script = putils::string<KENGINE_MAX_LUA_SCRIPT_PATH>;
using script_vector = putils::vector<script, KENGINE_MAX_LUA_SCRIPTS>;
```

##### Constructor

```cpp
LuaComponent(const script_vector &scripts = {});
```

##### attachScript

```cpp
void attachScript(const char * file) noexcept;
```

##### removeScript

```cpp
void removeScript(const char * file) noexcept;
```

##### getScripts

```cpp
const script_vector &getScripts() const noexcept;
```

# [LuaMetaComponent](LuaComponent.hpp)

### Specs

`Component` that holds a lua table that can be used to store arbitrary information in the `Entity`.

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Members

##### table

```cpp
sol::table table;
```

Lua scripts can set this value to anything they desire.
