# [ImGuiComponent](ImGuiComponent.hpp)

`Component` that provides a function to be called by the rendering system to display ImGui elements.

## Specs

* [Reflectible](https://github.com/phiste/putils/blob/master/reflection.md)
* Not serializable as it contains a functor
* Processed by graphics systems (such as the [OpenGLSystem](../../systems/OpenGLSystem.md))

## Usage

Once a graphics system handling ImGui has been registered, new elements can simply be added like so:

```cpp
EntityManager em;

em += [](Entity & e) {
    e += ImGuiComponent([] {
        if (ImGui::Begin("My window"))
            ImGui::Text("Content");
        ImGui::End();
    });
};
```

## Members

### Constructor

```cpp
template<typename Func> // Func: void()
ImGuiComponent(Func && func);
```

### setFunc

```cpp
template<typename Func> // Func: void()
void setFunc(Func && func);
```

This wraps `func` with a call used to set up the ImGui context (which might not be active in the current DLL).

`func` is stored as a [putils::function](putils/function.hpp). Its maximum size defaults to 64 and can be adjusted by defining the `KENGINE_IMGUI_FUNCTION_SIZE` macro.

### display

```cpp
putils::function<void(void * context), KENGINE_IMGUI_FUNCTION_SIZE> display;
```

Functor that will be called by the graphics system. Lua or Python scripts wanting to render things in ImGui will have to set this explicitly, and remember to call `ImGuiComponent::setupImGuiContext` before doing any rendering.