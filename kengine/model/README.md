# kengine_model

Framework for [flyweight](https://www.wikiwand.com/en/Flyweight_pattern) pattern implementations. The [instance](data/instance.md) component indicates that the entity is an *instance* of another. 

Entities referenced by instances are called `model entities`. They hold shared data about all instances.

* [README](README.md)
* [data](data)
	* [instance](data/instance.md): mark an entity as an instance of another
* [helpers](helpers)
	* [get](helpers/get.md): get the model's component
	* [has](helpers/model.md): check if the model has a component
	* [try_get](helpers/try_get.md): attempt to get the model's component

Sub-libaries:
* [kengine_model_find](find): system template to find an entity's model by a given component