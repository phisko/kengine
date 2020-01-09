# [LuaComponent](LuaComponent.hpp)

`Component` that specifies the list of lua scripts to be run for the `Entity` it is attached to.

Scripts can use the `self` global variable to access the `Entity` they are attached to.

In all the following functions, a `script` is the path to a file containing a script, NOT THE ACTUAL SCRIPT CODE.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by the [LuaSystem](../../systems/LuaSystem.md)

## Members

### Types

```cpp
using script = putils::string<KENGINE_MAX_LUA_SCRIPT_PATH>;
using script_vector = putils::vector<script, KENGINE_MAX_LUA_SCRIPTS>;
```

### scripts

```cpp
script_vector scripts;
```

The maximum length of a script name (stored as a [putils::string](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_MAX_LUA_SCRIPT_PATH` macro.

The maximum number of scripts defaults to 8 and can be adjusted by defining the `KENGINE_MAX_LUA_SCRIPTS` macro.