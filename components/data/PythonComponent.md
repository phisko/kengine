# [PythonComponent](PythonComponent.hpp)

Provides a set of Python scripts to execute for an `Entity`.

In Python, all the usual [script functions](../../helpers/scriptLanguageHelper.md) are stored in a `kengine` namespace (e.g. `pk.getEntity(id)`).

Scripts can use the `pk.self` global variable to access the `Entity` they are attached to.

The global layout of the component is very similar to that of the [LuaComponent](LuaComponent.md).

### Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (POD)
* Processed by the [PythonSystem](../../systems/python/PythonSystem.md)

The maximum length of a script name (stored as a [putils::string](https://github.com/phisko/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_MAX_PYTHON_SCRIPT_PATH` macro.

The maximum number of scripts defaults to 8 and can be adjusted by defining the `KENGINE_MAX_PYTHON_SCRIPTS` macro.