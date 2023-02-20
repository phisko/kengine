# kengine_core

Components and helpers that are accessed by most (if not all) other libraries.

* [data](data/)
	* [name](data/name.md): an entity's name
	* [selected](data/selected.md): tags an entity as selected by the user
	* [transform](data/transform.md): an entity's position, rotation and scale
* [helpers](helpers/)
	* [new_entity_processor](helpers/new_entity_processor.md): automatically call a functor when entities enter a group

Sub-libraries:

* [kengine_core_assert](assert/): engine-level assertions
* [kengine_core_log](log/): generic logging support
* [kengine_core_profiling](kengine/core/profiling/): profiling using [Tracy](https://github.com/wolfpld/tracy)
* [kengine_core_sort](kengine/core/sort/): entity sorting helpers