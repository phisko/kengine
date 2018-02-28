# [Box2DSystem](Box2DSystem.hpp)

`System` that moves objects according to the information found in their [Box2DComponent](../components/Box2DComponent.md).

### Behavior

At each step, the `Box2DSystem` moves each `GameObject` with a `Box2DComponent` according to the component's information, adjusting the values according to the framerate and elapsed time.

If two objects overlap at one point or another, a [Collision](../packets/Collision.hpp) packet is sent out, letting other `Systems` deal with the event.

### Queries

The `Box2DSystem` can be used to query the list of `GameObjects` found within an area using the [Position](../packets/Position.hpp) query.
