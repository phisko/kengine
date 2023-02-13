# [create_system](create_system.hpp)

`Function component` that creates a system.

## Prototype

```cpp
entt::entity (entt::registry &);
```

### Parameters

* `r`: the registry in which the system should be created

## Usage

System implementers can use the [system_creator_helper](../helpers/system_creator_helper.md) to define standard `add_[system]` and `register_[system]` functions that will create an entity with a `create_system` component, which can later be used by client code to create (or re-create) a system.

The [create_all_systems](../helpers/create_all_systems.md) helper will call all `create_system` components for a given registry, creating all pre-registered systems.