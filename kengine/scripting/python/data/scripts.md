# [python](python.hpp)

Component that runs Python scripts.

In Python, all the usual [script functions](../../helpers/script_language_helper.md) are stored in a `kengine` namespace (e.g. `kengine.get_entity(id)`).

Scripts can use the `kengine.self` global variable to access the entity they are attached to.

The global layout of the component is very similar to that of the [lua component](../../lua/data/lua.md).

The maximum length of a script name (stored as a [putils::string](https://github.com/phisko/putils/blob/master/putils/string.md)) defaults to 64, and can be adjusted by defining the `KENGINE_MAX_PYTHON_SCRIPT_PATH` macro.

The maximum number of scripts defaults to 8 and can be adjusted by defining the `KENGINE_MAX_PYTHON_SCRIPTS` macro.