# [lua](lua.hpp)

Component that runs Lua scripts.

Scripts can use the `self` global variable to access the entity they are attached to.

In all the following functions, a `script` is the path to a file containing a script, NOT the actual script code.

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

The maximum length of a script name (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 64, and can be adjusted by defining the `KENGINE_MAX_LUA_SCRIPT_PATH` macro.

The maximum number of scripts defaults to 8 and can be adjusted by defining the `KENGINE_MAX_LUA_SCRIPTS` macro.