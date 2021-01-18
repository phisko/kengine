# [Entities](Entities.hpp)

Object that lets clients create, remove, access and iterate over [Entity](Entity.md) objects.

## Members

### create

```cpp
template<typename Func> // Func: EntityCreator
Entity create(Func && postCreate) noexcept;

template<typename Func>
Entity operator+=(Func && postCreate) noexcept;
```

Creates an `Entity` and passes it to `postCreate`, which can be used to attach `Components`. [OnEntityCreated](components/functions/OnEntityCreated.md) is then called on all `Entities` which have it.

### remove

```cpp
void remove(Entity e) noexcept;
void remove(EntityID id) noexcept;
void operator-=(Entity e) noexcept;
void operator-=(EntityID id) noexcept;
```

Removes an `Entity`, and calls [OnEntityRemoved](components/functions/OnEntityRemoved.md) on all `Entities` which have it.

### get

```cpp
Entity get(EntityID id) noexcept;
Entity operator[](EntityID id) noexcept;
```

Returns the requested `Entity`.

### setActive

```cpp
void setActive(Entity e, bool active) noexcept;
void setActive(EntityID id, bool active) noexcept;
```

Marks an `Entity` as "active" or not. Inactive `Entities` will be skipped by iterators.

### with

```cpp
template<typename ... Comps>
impl::ComponentCollection<Comps...> with() const noexcept;
```

Returns a collection of all `Entities` with the desired `Components`. Dereferencing an iterator of this collection returns an `std::tuple<Entity, Comps & ...>`. For instance:

```cpp
for (const std::tuple<Entity, TransformComponent &, NameComponent &> & tuple : entities.with<TransformComponent, NameComponent>()) {
    // ...
}

// more readable
for (auto [e, transform, name] : entities.with<TransformComponent, NameComponent>()) {
    // ...
}
```

### begin, end

```cpp
impl::EntityIterator begin() const noexcept;
impl::EntityIterator end() const noexcept;
```

Lets you iterate over all active `Entities`. Dereferencing an iterator returns an `Entity`.