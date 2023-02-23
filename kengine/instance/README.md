# kengine_instance

Framework for [flyweight](https://www.wikiwand.com/en/Flyweight_pattern) pattern implementations. The [instance](data/instance.md) component indicates that the entity is an *instance* of another. 

Entities referenced by instances are called `model entities`. They hold shared data about all instances.

* [README](README.md)
* [data](data)
	* [instance](data/instance.md): mark an entity as an instance of another
* [helpers](helpers)
	* [get_model](helpers/get_model.md): get the model's component
	* [model_has](helpers/model_has.md): check if the model has a component
	* [try_get_model](helpers/try_get_model.md): attempt to get the model's component

Sub-libraries:
* [kengine_instance_find_model_by_name](find_model_by_name): find an entity's model by its name