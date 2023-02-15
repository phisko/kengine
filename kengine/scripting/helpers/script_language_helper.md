# [script_language_helper](script_language_helper.hpp)

Set of helper functions meant to be used by language-specific scripting systems (e.g. the [lua system](../lua/systems/lua.md) or the [python system](../python/systems/python.md)).

## Members

All these functions are only meant for internal use by the scripting systems.

### init

```cpp
template<typename Func, typename Func2>
void init(entt::registry & r, Func && register_function, Func2 && register_type) noexcept;
```

Registers global entity manipulation functions. Calls `register_type` with `putils::meta::type<Entity>` as parameter.

### register_component

```cpp
template<typename T, typename Func>
void register_component(entt::registry & r, Func && register_entity_member) noexcept;
```

Calls `register_entity_member` to register functions that will manipulate the `T` component.

This lets scripts perform any operation on entities if the necessary types are registered. Client code can either give full access to scripts by registering all its types (and therefore having a fully extensible game that can be developed almost entirely in a scripting language), or only register a small set of types and/or members, to restrict what modders can do.
