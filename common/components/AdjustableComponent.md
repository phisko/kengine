# [AdjustableComponent](AdjustableComponent.hpp)

Utility `Component` that provides access to "global" values that may be adjusted at runtime, e.g. by an [ImGuiAdjustableManager](../gameobjects/ImGuiAdjustableManager.md).

Currently supported types are `bool`, `int`, `float` and `string`.

Inherits from [putils::Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md).

### Usage

```c++
bool globalValue = false;

em += [&](kengine::Entity e) {
    e += kengine::AdjustableComponent("Adjustable value name", &globalValue);
};

em.execute(); // Draws the adjustable interface, lets users modify adjustable
std::cout << globalValue << std::endl; // May be true, if user modified it
```
