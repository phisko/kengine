# [GlobalState](GlobalState.hpp)

Holds global state for the entire engine.

## Members

### threadPool

```cpp
putils::ThreadPool threadPool;
```

Thread pool used by all systems to run asynchronous tasks in a controlled manner.

### running

```cpp
std::atomic<bool> running = true;
```

Indicates whether systems should keep running or stop.

### _entities

```cpp
struct EntityMetadata {
    struct {
        bool active = false;
        ComponentMask mask = 0;
        bool shouldActivateAfterInit = true;
    };
    ID freeListNext;
};
std::vector<EntityMetadata> _entities;
Mutex _entitiesMutex;
```

List of all existing [Entities](../Entity.md). Either holds the `Entity`'s information or the `ID` of the next `Entity` to be reused.

### _archetypes

```cpp
std::vector<Archetype> _archetypes;
Mutex _archetypesMutex;
```

List of all existing [Archetypes](Archetype.md).

### _freeList

```cpp
ID _freeList;
Mutex _freeListMutex;
```

Index of the next `Entity` to be reused.

### _components

```cpp
std::vector<std::unique_ptr<ComponentMetadata>> _components;
Mutex _componentsMutex;
```

List of all existing [ComponentMetadata](ComponentMetadata.md).

## Free function

```cpp
Entity alloc() noexcept;
```

Creates a new `Entity`, given a unique `id`.