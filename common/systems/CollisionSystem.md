# [CollisionSystem](CollisionSystem.hpp)

`System` that listens for [Collision](../packets/Collision.hpp) packets and checks each of the involved `GameObjects` for a [CollisionComponent](../components/CollisionComponent.hpp). If either of the `GameObjects` is found to have a `CollisionComponent`, its `onCollide` member function is called with the two objects. The first parameter to `onCollide` is always the `GameObject` to which the `CollisionComponent` is attached.

### Behavior

Whenever a `Collision` packet is received, the `CollisionSystem` performs the following for each `GameObject` involved in the collision:

* If the `GameObject` has a `CollisionComponent`
    * Call `onCollision(go, other)`, with `go` and `other` being the references to the two `GameObjects` involved.

