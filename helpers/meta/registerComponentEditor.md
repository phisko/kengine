# [registerComponentEditor](registerComponentEditor.hpp)

Helper functions to register sample implementations of the [DisplayImGui](../components/meta/DisplayImGui.md) and [EditImGui](../components/meta/EditImGui.md) `meta Components`.

## Members

### registerComponentEditor

```cpp
template<typename Comp>
void registerComponentEditor(EntityManager & em);
```

Implements the `DisplayImGui` and `EditImGui` `meta Components` for `Comp`.

### registerComponentEditors

```cpp
template<typename ... Comps>
void registerComponentEditors(EntityManager & em);
```

Calls `registerComponentEditor<T>` for each `T` in `Comps`.
