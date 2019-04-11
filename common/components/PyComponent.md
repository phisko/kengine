# [PyComponent](PyComponent.hpp)

Provides a set of Python scripts to execute for an `Entity`.

All the usual [script functions](../systems/ScriptSystem.md) are stored in a `pk` namespace (e.g. `pk.getEntity(id)`).

Scripts can use the `pk.self` global variable to access the `Entity` they are attached to.

The global layout of the component is very similar to that of the [LuaComponent](LuaComponent.md).

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Serializable

The maximum length of a script name (stored as a [putils::string](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_MAX_PYTHON_SCRIPT_PATH` macro.

The maximum number of scripts defaults to 8 and can be adjusted by defining the `KENGINE_MAX_PYTHON_SCRIPTS` macro.