# [EntityManager](EntityManager.hpp)

Manages [Entities](Entity.md), [Components](Component.md) and [Systems](System.md).

An `EntityManager` is also a [Mediator](https://github.com/phisko/putils/blob/master/mediator/README.md), managing communication for `Systems`.

### Base classes

An `EntityManager`'s role is split-up into two parts:

* the `EntityManager` itself, which manages entities and their components
* a [SystemManager](SystemManager.md) base

### Members

##### Constructor

```cpp
EntityManager(size_t threads = 0);
```
An `EntityManager` can be constructed with a number of threads, which will be used for its [ThreadPool](https://github.com/phisko/putils/blob/master/ThreadPool.hpp).

##### createEntity

```cpp
template<typename Func> // Func: void(Entity);
Entity &createEntity(Func && postCreate);
```

Creates a new `Entity`, calls `postCreate` on it, and registers it to the existing `Systems`.

##### removeEntity

```cpp
void removeEntity(kengine::EntityView e);
void removeEntity(Entity::ID id);
```

##### getEntity

```cpp
Entity &getEntity(Entity::ID id);
```

##### getEntities

```cpp
auto getEntities();
```

Returns an iteratable collection over all `Entities`.

```cpp
template<typename ...Comps>
auto getEntities<Comps...>();
```

Returns and iteratable collection over all `Entities` which have each component listed in `Comp`.

Dereferencing the iterator returns an `std::tuple<EntityView, Comps &...>`, meaning you can write the following:
```cpp
for (const auto & [e, transform, lua] : em.getEntities<TransformComponent3f, LuaComponent>())
```

##### pause

```cpp
void pause();
```

Used to pause the game by setting the `speed` to 0. Calling `isPaused` from a [System](System.md) will return `true`, and `time.getDeltaFrames()` will always return 0. This means that most systems do not have to take any special measures to adapt to the game being paused, as long as they use `time.getDeltaFrames()` to adapt their behavior. The [PhysicsSystem](common/systems/PhysicsSystem.md), for instance, multiplies the `Entities`' movement by `time.getDeltaFrames()` to accomodate for dropped frames, and will therefore automatically stop moving them when the game is paused.

##### resume

```cpp
void resume();
```

Used to resume the game by setting the `speed` to 1.

##### setSpeed

```cpp
void setSpeed(double speed);
```

Sets the game's speed to `speed`. This will impact the return value of `time.getDeltaFrames()` in `Systems`, letting them automatically adjust their behavior if they take framerate into account.

##### getSpeed

```cpp
double getSpeed() const;
```

Returns the game's speed.
