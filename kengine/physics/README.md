# kengine_physics

Components and systems to handle moving objects.

* [data](data)
	* [inertia](data/inertia.md): applies movement to an entity
	* [model_collider](data/model_collider.md): [model component](../model/) listing the model's colliders
* [functions](functions)
	* [on_collision](functions/on_collision.md): called whenever two entities collide
	* [query_position](functions/query_position.md): returns a list of entities around a position

Sub-libraries:
* [kengine_physics_bullet](bullet): use Bullet to simulate physics
* [kengine_physics_collision](collision): notify entities when they collide with others
* [kengine_physics_kinematic](kinematic): handle hand-moved objects