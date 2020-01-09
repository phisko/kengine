# [KinematicComponent](KinematicComponent.hpp)

`Component` that marks an `Entity` as kinematic, i.e. "hand-moved" and not managed by physics systems.

## Specs

* [Reflectible](https://github.com/phisko/putils/blob/master/reflection.md)
* Serializable (empty, only a tag)
* Taken into account by the [BulletSystem](../../systems/bullet/BulletSystem.md) and the [KinematicSystem](../../systems/KinematicSystem.md)