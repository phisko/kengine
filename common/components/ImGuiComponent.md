# [ImGuiComponent](ImGuiComponent.hpp)

`Component` that provides a function to be called by the rendering `System` to display ImGui elements.

### Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection/Reflectible.md)
* Not serializable

### Members

##### Constructor

```cpp
template<typename Func> // Func: void()
ImGuiComponent(Func && func);
```

##### setFunc

```cpp
template<typename Func> // Func: void()
void setFunc(Func && func);
```

This wraps `func` with a call used to set up the ImGui context.

`func` is stored as a [putils::function](putils/function.hpp). Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_IMGUI_FUNCTION_SIZE` macro.