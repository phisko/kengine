# kengine_meta

Components and helpers to setup and manipulate runtime reflection facilities for components.

* [data](data)
	* [attributes](data/attributes.md): lists a type's attributes
	* [size](data/size.md): holds a type's size
* [functions](functions)
	* [count](functions/count.md): counts the number of entities with the component
	* [emplace_or_replace](functions/emplace_or_replace.md): adds the component to an entity
	* [for_each_entity](functions/for_each_entity.md): iterates all entities with the component
	* [get](functions/get.md): returns the component in an entity
	* [has](functions/has.md): returns whether an entity has the component
	* [match_string](functions/match_string.md): returns whether the component in an entity matches a string
	* [register_types](functions/register_types.md): registers types with a `registry`
	* [remove](functions/remove.md): removes the component from an entity
* [helpers](helpers)
	* [find_attribute](helpers/find_attribute.md)
	* [get_type_entity](helpers/get_type_entity.md)
	* [register_all_types](helpers/register_all_types.md): call all `register_type` functions
	* [register_everything](helpers/register_everything.md): register everything for a set of types
	* [register_metadata](helpers/register_metadata.md): register metadata for a set of types
	* [register_meta_components](helpers/register_meta_components.md): register meta components for a set of types
	* [register_meta_component_implementation](helpers/register_meta_component_implementation.md): register the implementation for a meta component
	* [register_storage](helpers/register_storage.md): pre-instantiate storage for a set of types
	* [register_with_script_languages](helpers/register_with_script_languages.md): register a set of types with script languages
	* [impl](helpers/impl): meta component implementations

Sub-libraries:
* [kengine_meta_imgui](imgui)
* [kengine_meta_json](json)
