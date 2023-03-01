# kengine_scripting

Systems to provide bindings to scripting languages, and automatically run scripts in those languages.

* [data](data)
	* [scripts](data/scripts.md): list of scripts to run for an entity
* [helpers](helpers)
	* [init_bindings](helpers/init_bindings.md): initialize the bindings for a language
	* [register_component](helpers/register_component.md): register a component with a language

Sub-libraries:
* [kengine_scripting_imgui_prompt](imgui_prompt): display an ImGui window with a prompt to evaluate expressions
* [kengine_scripting_lua](lua): Lua bindings
* [kengine_scripting_python](python): Python bindings