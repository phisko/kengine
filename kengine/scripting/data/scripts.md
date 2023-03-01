# [scripts](scripts.hpp)

Scripts to run for an entity.

## Members

### Types

```cpp
using script = putils::string<KENGINE_SCRIPTING_MAX_SCRIPT_PATH>;
using script_vector = putils::vector<script, KENGINE_SCRIPTING_MAX_SCRIPTS>;
```

### scripts

```cpp
script_vector files;
```

The maximum length of a script name (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 64, and can be adjusted by defining the `KENGINE_SCRIPTING_MAX_SCRIPT_PATH` macro.

The maximum number of scripts defaults to 8 and can be adjusted by defining the `KENGINE_SCRIPTING_MAX_SCRIPTS` macro.
