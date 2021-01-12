# [ComponentMetadata](ComponentMetadata.hpp)

Holds implementation details for a given Component.

## Members

### id

```cpp
ID id = INVALID_ID;
```

Unique identifier for the `Component` type.

### type

```cpp
putils::meta::type_index type;
```

Index of the `Component` type. Used so that plugins can check whether a `ComponentMetadata` already exists for a given type before creating it.

### reset

```cpp
virtual void reset(ID id) noexcept = 0;
```

Resets the `Component` attached to a given `Entity`.

### _map

```cpp
componentSettings::map<Comp> _map;
```

Maps an `EntityID` to its `Component`. The type depends on what the user has specified in the `Component`'s [ComponentSettings](../ComponentSettings.md).

### _mutex

```cpp
Mutex _mutex;
```

Used internally.