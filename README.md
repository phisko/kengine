# Kengine

The Koala engine is a type-safe and self-documenting implementation of an Entity-Component-System (ECS).

### Classes

* [Component](Component.md): contains information about a certain property of an entity (for instance, a `TransformComponent` might hold an entity's position and size)
* [GameObject](GameObject.md): represents an in-game entity. Is simply a container of `Components`
* [System](System.md): holds game logic. A `PhysicsSystem` might control the movement of `GameObjects`, for instance.
* [EntityManager](EntityManager.md): manages `GameObjects`, `Components` and `Systems`
