# System

A system holds game logic. It can subscribe to a certain type of [Component](Component.md), in which case it will automatically `register` all [GameObjects](GameObject.md) with a `Component` of that type.

`Systems` are also [Modules](putils/mediator/README.md), the `EntityManager` being their `Mediator`. This lets `Systems` easily communicate through `DataPackets`.

### Members

##### Definition

```
template<typename CRTP, typename RegisteredComponent = IgnoreComponents, typename ...DataPackets>
class System;
```

A `System` is defined by its sub-type (see `CRTP`), the type of `Component` it would like to register to (none by default), and the list of `DataPackets` types it would like to receive.

##### execute

```
virtual void execute() = 0;
```

Runs the system's game logic. Called each frame.

##### registerGameObject

```
void registerGameObject(GameObject &go);
```

Automatically called when an "interesting" `GameObject` is added (one with the type of `Component` this system is registered to).

By default, this simply adds the `GameObject` to an `std::vector` which can be accessed through `getGameObjects()`. Inheriting classes may override it to perform an action upon registration.

##### removeGameObject

```
void removeGameObject(GameObject &go);
```

Automatically called when an "interesting" `GameObject` is removed (one with the type of `Component` this system is registered to).

By default, this simply removes the `GameObject` from an `std::vector` which can be accessed through `getGameObjects()`. Inheriting classes may override it to perform an action upon removal.

##### getGameObjects

```
std::vector<GameObject*> &getGameObjects() { return _gameObjects; }
const std::vector<GameObject*> &getGameObjects() const { return _gameObjects; }
```

Provides access to the list of `GameObjects` which have been registered.
