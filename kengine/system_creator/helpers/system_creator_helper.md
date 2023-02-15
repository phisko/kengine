# [system_creator_helper](system_creator_helper.hpp)

Helper macros to define standard `add_[system]` and `register_[system]` functions.

## Resulting functions

### add_[system_name]

```cpp
entt::entity add_[system_name](entt::registry & r) noexcept;
```

Calls `register_[system_name]`, and immediately attaches the system to the created entity.

### register_[system_name]

```cpp
entt::entity register_[system_name](entt::registry & r) noexcept;
```

Calls [register_storage](../../meta/helpers/register_storage.md) for each type used by the system (including the system itself).

Creates an entity `e` with a [create_system](../functions/create_system.md) component that will:
* attach the system to `e` if called with `e`'s owning registry
* call `add_system` if not, thus creating a separate "system entity" in the other registry

## Members

### DECLARE_KENGINE_SYSTEM_CREATOR

```cpp
#define DECLARE_KENGINE_SYSTEM_CREATOR(EXPORT_MACRO, system_name)
```

Declares the functions, prefixed with the `EXPORT_MACRO` macro for symbol specification.

### DEFINE_KENGINE_SYSTEM_CREATOR

```cpp
#define DEFINE_KENGINE_SYSTEM_CREATOR(system_name, ...)
```

Defines the functions. The variadic arguments should be all the components used by the system, so that their [storage can be registered](../../meta/helpers/register_storage.md).