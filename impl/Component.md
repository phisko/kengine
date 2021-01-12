# [Component](Component.hpp)

Low-level `Component` handling.

## Members

### get

```cpp
static Comp & get(ID entity) noexcept;
```

Returns the `Component` for the required `entity`. If none exists, creates it.

### id

```cpp
static ID id() noexcept;
```

Returns the unique identifier for this `Component` type. This is later used as an index into [Entities'](../Entity.md) `componentMask`.

## Free functions

```cpp
void addComponent(EntityID entity, ComponentID component) noexcept;
void removeComponent(EntityID entity, ComponentID component) noexcept;
```

Attaches or detaches a component from an entity.