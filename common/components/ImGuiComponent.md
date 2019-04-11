# [ImGuiComponent](ImGuiComponent.hpp)

`Component` that provides a function to be called by the rendering `System` to display ImGui elements.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Members

##### Constructor

```cpp
ImGuiComponent(const std::function<void()> & func);
```

##### setFunc

```cpp
void setFunc(const std::function<void()> & func);
```

This wraps `func` with a call used to set up the ImGui context.