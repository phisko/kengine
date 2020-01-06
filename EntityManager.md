# [EntityManager](EntityManager.hpp)

Provides access to [Entities](Entity.md).

## Members

### Constructor

```cpp
EntityManager(size_t threads = 0);
```
An `EntityManager` can be constructed with a number of threads, which will be used for its [ThreadPool](https://github.com/phisko/putils/blob/master/ThreadPool.hpp).

### createEntity

```cpp
template<typename Func> // Func: void(Entity &);
Entity createEntity(Func && postCreate);
```

Creates a new `Entity`, calls `postCreate` on it, and registers it to the existing `Systems`.

### operator+=

```cpp
template<typename Func> // Func: void(Entity &)
Entity operator+=(Func && postCreate);
```

Equivalent to `createEntity`. Allows for syntax such as:

```cpp
em += [](Entity & e) {
  e += TransformComponent{};
};
```

### removeEntity

```cpp
void removeEntity(Entity e);
void removeEntity(Entity::ID id);
```

### getEntity

```cpp
Entity getEntity(Entity::ID id);
```

### getEntities

```cpp
auto getEntities();
```

Returns an iteratable collection over all `Entities`.

```cpp
template<typename ...Comps>
auto getEntities<Comps...>();
```

Returns an iteratable collection over all `Entities` which have each component listed in `Comps`.

Dereferencing the iterator returns an `std::tuple<Entity, Comps &...>`, which means you can write the following:
```cpp
for (const auto & [e, transform, lua] : em.getEntities<TransformComponent, LuaComponent>()) {
    std::cout << "Pitch: " << transform.pitch << '\n';
    // do stuff
}
```

### no

```cpp
template<typename T>
struct no;
```

Can be used as a template parameter for `getEntities<Comps...>` to filter out `Entities` which have a specific `Component`:

```cpp
for (const auto & [e, transform, noSelected] : em.getEntities<TransformComponent, no<SelectedComponent>>) {
    // Entities with a SelectedComponent will be filtered out
    std::cout << e.id << " has a TransformComponent but no SelectedComponent" << '\n';
}
```