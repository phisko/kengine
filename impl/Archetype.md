# [Archetype](Archetype.hpp)

Collection of [Entities](../Entity.md) which have the same set of `Components`. When iterating over `Entities`, [ComponentCollections](ComponentCollection.md) use these archetypes to avoid useless iterations.

## Members

### mask

```cpp
ComponentMask mask;
```

Bitset indicating which `Components` these entities have.

### entities

```cpp
std::vector<ID> entities;
```

Lists the entities matching `mask`.

### sorted, mutex

```cpp
bool sorted = true;
mutable Mutex mutex;
```

Used internally.

### matches

```cpp
template<typename ... Comps>
bool matches() noexcept;
```

Returns whether this archetype's entities have all the `Components` listed in `Comps`.

### add, remove, sort

```cpp
void add(EntityID id) noexcept;
void remove(EntityID id) noexcept;
void sort() noexcept;
```