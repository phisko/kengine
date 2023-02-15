# [collision](collision.hpp)

System that listens for [on_collision](../../functions/on_collision.md) events and checks each of the involved entities for a [collision](../data/collision.md). If either of the entities is found to have a `collision` component, its `on_collide` member function is called with the two objects.

## Behavior

Whenever the `on_collision` `function component` is called, the system performs the following for each entity involved in the collision:

* If the entity has a `collision` component
    * Call `on_collision(e, other)`, with `e` and `other` being the two entities involved.

