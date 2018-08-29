# [AdjustableComponent](AdjustableComponent.hpp)

Utility `Component` that provides access to "global" values that may be adjusted at runtime, e.g. by an [ImGuiAdjustableSystem](../systems/ImGuiAdjustableSystem.md).

Currently supported types are `bool`, `int`, `double` and `string`.

Inherits from [putils::Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md).

### Usage

Here is an example of creating two different adjustables: one of them using a pointer to a global value that will be modified directly by the adjustable system, while the other simply contains its value. Which method you use is up to you.

```c++
bool globalValue = false;

em.createOrAttach<kengine::AdjustableComponent>("entityName", "adjustableValueName", &globalValue); // Adjustable will point to globalValue
em.execute(); // Draws the adjustable interface, lets users modify adjustable
std::cout << globalValue << std::endl; // May be true, if user modified it

auto & go = em.createOrAttach<kengine::AdjustableComponent>("entityName2", "adjustableValueName2", 42); // Adjustable value will be held in component, set to 42
em.execute();
std::cout << go.getComponent<kengine::AdjustableComponent>().i; // i for int
```
