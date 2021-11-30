# [CollisionSystem](CollisionSystem.hpp)

`System` that listens for [Collision](../components/functions/OnCollision.md) events and checks each of the involved `Entities` for a [CollisionComponent](../components/data/CollisionComponent.hpp). If either of the `Entities` is found to have a `CollisionComponent`, its `onCollide` member function is called with the two objects. The first parameter to `onCollide` is always the `Entity` to which the `CollisionComponent` is attached.

## Behavior

Whenever the `OnCollision` `function Component` is called, the `CollisionSystem` performs the following for each `Entity` involved in the collision:

* If the `Entity` has a `CollisionComponent`
    * Call `onCollision(go, other)`, with `go` and `other` being the references to the two `Entities` involved.

