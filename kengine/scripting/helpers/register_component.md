# [register_component](register_component.hpp)

```cpp
template<typename T, typename Func>
void register_component(entt::registry & r, Func && register_entity_member) noexcept;
```

Calls `register_entity_member` to register functions that will manipulate the `T` component.

This lets scripts perform any operation on entities if the necessary types are registered. Client code can either give full access to scripts by registering all its types (and therefore having a fully extensible game that can be developed almost entirely in a scripting language), or only register a small set of types and/or members, to restrict what modders can do.