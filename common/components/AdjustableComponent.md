# [AdjustableComponent](AdjustableComponent.hpp)

Utility `Component` that provides access to "global" values that may be adjusted at runtime, e.g. by an [ImGuiAdjustableManager](../gameobjects/ImGuiAdjustableManager.md).

Currently supported types are `bool`, `int`, `float`.

The maximum length of the adjustable's name (stored as a [putils::string](https://github.com/phiste/putils/blob/master/string.hpp)) defaults to 64, and can be adjusted by defining the `KENGINE_ADJUSTABLE_NAME_MAX_LENGTH` macro.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Not serializable, but the adjustable system you use may externally save adjustable values

### Usage

```c++
bool globalValue = false;

em += [&](kengine::Entity e) {
    e += kengine::AdjustableComponent("Adjustable value name", &globalValue);
};

em.execute(); // Draws the adjustable interface, lets users modify adjustable
std::cout << globalValue << std::endl; // May be true, if user modified it
```
