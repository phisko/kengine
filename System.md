# System

A system holds game logic. It can subscribe to a certain type of [Component](Component.md), in which case it will automatically `register` all [GameObjects](GameObject.md) with a `Component` of that type.

`Systems` are also [Modules](putils/mediator/README.md), the `EntityManager` being their `Mediator`. This lets `Systems` easily communicate through `DataPackets`.

### Members

##### Definition

```
template<typename CRTP, typename ...DataPackets>
class System;
```

A `System` is defined by its sub-type (see `CRTP`) and the list of `DataPackets` types it would like to receive.

##### execute

```
virtual void execute() = 0;
```
Runs the system's game logic. Called each frame.

##### registerGameObject

```
virtual void registerGameObject(GameObject &go) {}
```
Automatically called for each new `GameObject`.

##### removeGameObject

```
virtual void removeGameObject(GameObject &go) {}
```
Automatically called for each `GameObject` that is removed.
