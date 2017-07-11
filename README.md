# Kengine

The Koala engine is a type-safe and self-documenting implementation of an Entity-Component-System (ECS).

### Classes

* [Component](Component.md): contains information about a certain property of an entity (for instance, a `TransformComponent` might hold an entity's position and size)
* [GameObject](GameObject.md): represents an in-game entity. Is simply a container of `Components`
* [System](System.md): holds game logic. A `PhysicsSystem` might control the movement of `GameObjects`, for instance.
* [EntityManager](EntityManager.md): manages `GameObjects`, `Components` and `Systems`
* [EntityFactory](EntityFactory.md): used to create `GameObjects` typed at run-time (by replacing template parameters by strings)

### Samples

These are pre-built, extensible and pluggable elements that can be used in any project, or to bootstrap a project.

##### Components

* [LuaComponent](common/components/LuaComponent.md): defines the lua scripts to be run by the `LuaSystem` for a `GameObject`
* [MetaComponent](common/components/MetaComponent.md): provides metadata about a `GameObject`, such as its appearance, used by the `SfSystem`
* [TransformComponent](common/components/TransformComponent.md): defines a `GameObject`'s position and size

##### Systems

* [LogSystem](common/systems/LogSystem.md): logs messages
* [LuaSystem](common/systems/LuaSystem.md): executes scripts, either global or attached to an entity
* [SfSystem](common/systems/sfml/SfSystem.md): displays entities in an SFML render window

##### DataPackets

* [Log](common/packets/Log.hpp): received by the `LogSystem`, used to log a message
* [RegisterAppearance](common/packets/RegisterAppearance.hpp): received by the `SfSystem`, maps an abstract appearance to a concrete texture file.

These are datapackets sent from one `System` to another to communicate.
