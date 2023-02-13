# Kengine 

[![tests](https://github.com/phisko/kengine/workflows/tests/badge.svg)](https://github.com/phisko/kengine/actions/workflows/tests.yml)

The Koala engine is a game engine entirely implemented as an [Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system) (ECS).

The engine is based on [EnTT](https://github.com/skypjack/entt/). Integration with other pieces of software using `EnTT` should be straightforward. This documentation assumes at least basic knowledge of `EnTT` and its terminology (`entity`, `registry`, `handle`...).

![koala](koala.png)

## Example

The [example project](example/) showcases some of the core features. It should give you an idea of what the engine's support for reflection and runtime extensibility have to offer.

## Installation

The engine uses Git submodules, and should therefore be cloned recursively with
```
git clone https://github.com/phisko/kengine --recursive
```

The engine has been tested on Windows with MSVC and MinGW. MinGW is a bit finnicky, as some build configurations simply don't work. This is mostly due to external dependencies pulled from [conan](https://conan.io/), which could be built from source in order to fix the errors. At the time of writing I don't have a personal need for it, so I haven't gone out of my way to do so.

Linux compilation works with GCC. At the time of writing, clang doesn't support C++ 20's `constexpr std::string` and `std::vector`.

## C++ version

The engine requires a **C++20** compiler.

## Foreword

The engine started as a passion/student project in 2016-17. My friends/colleagues and I had a go at implementing an ECS from the ground up. We wanted absolute type safety and clarity, and while we'd already toyed with template metaprogramming, this was a chance for us to learn more about it.

Once the core ECS was working, the engine turned into a playground to learn more about game development. I learned OpenGL rendering, how to use navmeshes with Recast/Detour, setup physics with Bullet... All the while developing useful helpers and runtime reflection facilities compatible with an ECS.

After now over 5 years working on the engine, I realized the core ECS itself is no longer the focus of this project. Other libraries, and `EnTT` in particular, offers a very similar API, with much more advanced features. So I took the time to completely gut out the internal ECS and replace it with `EnTT`. All features remain the same, and this will give me more time to work on useful helpers, which can work with other `EnTT` projects.

## Reflection

Many parts of the engine (such as the scripting systems or the ImGui entity editor) make use of `putils`' [reflection API](https://github.com/phisko/reflection). Most of the components in the following samples are thus defined as reflectible.

## Layout

The engine is organized in [components](#components), [systems](#systems) and [helpers](#helpers) .

Note that `systems` aren't objects of a specific class. Systems are simply functions that create an entity with an [execute](kengine/functions/execute.md) component (or anything else they need to do their work). The entity then lives in the `registry` with the rest of the game state. This lets users introspect systems or add behavior to them just like any other entity.

## Component categories

The engine comes with a (fairly large) number of pre-built components that can be used to bootstrap a game, or simply as examples that you can base your own implementations upon.

These components fit into three categories:
* Data components
* Function components
* Meta components

### Data components

Data components hold data about their entity.

Data components are what first comes to mind when thinking of a component, such as a [transform](kengine/data/transform.md) or a [name](kengine/data/name.md).

Data components can sometimes hold functions:
* [input](kengine/data/input.md) lets an entity hold callbacks to be called whenever an input event occurs
* [collision](kengine/data/collision.md) lets an entity be notified when it collides with another

### Function components

Function components hold functions to query, alter, or notify their entity.

Function components are simply holders for functors that can be attached as components to entities. This mechanic can be used to:
* attach behaviors to entities: [execute](kengine/functions/execute.md) is called by the main loop each frame
* register callbacks for system-wide events: [on_click](kengine/functions/on_click.md) is called whenever the user clicks the window
* provide new functionality that is implemented in a specific system: [query_position](kengine/functions/query_position.md) can only be implemented in a physics system

Function components are types that inherit from [base_function](kengine/base_function.hpp), giving it the function signature as a template parameter.

To call a function component, one can use its `operator()` or its `call` function.

```cpp
entt::registry r;
const auto e = r.create();
r.emplace<functions::Execute>(e,
    [](float delta_time) { std::cout << "Yay!" << std::endl; }
);

const auto & execute = r.get<functions::execute>(e); // Get the function
execute(0.f); // Call it with its parameters
execute.call(42.f); // Alternatively
```

### Meta components

Meta components are components for components.

The engine uses "type entities" to hold information about the various components in use. Each type entity represents a different component type, and can be used to query the component's properties at runtime.

Meta components are attached to these "type entities", and hold a generic function's implementation for that specific type. Because they hold functions, they are very similar to function components.

An example makes this clearer: [edit_imgui](kengine/meta/edit_imgui.md) is a meta component that, when called, will draw its "parent component"'s properties using [ImGui](https://github.com/ocornut/imgui/) for the given entity. The following code will display a window to edit `e`'s [name](kengine/data/name.md) component.

```cpp
// r is a registry with the "type entity" for `name` already setup

const auto e = r.create();
r.emplace<data::name>(e);

const auto type_entity = type_helper::get_type_entity<data::name>(r);
const auto & edit_imgui = r.get<meta::edit_imgui>(type_entity);
if (ImGui::Begin("Edit name"))
    edit_imgui({ r, e });
ImGui::End();
```

If you generalize this, you can edit all the components for an entity with the following code:

```cpp
// r is a registry with the "type entities" for all used components already setup
// e is an entity with an unknown set of components

if (ImGui::Begin("Edit entity"))
    for (const auto & [type_entity, edit_imgui] : r.view<meta::edit_imgui>()) {
        edit_imgui({ r, e });
    }
ImGui::End();
```

## Components

### [Data components](kengine/data/)

#### General purpose

* [command_line](kengine/data/command_line.md): holds the command-line arguments
* [instance](kengine/data/instance.md): specifies the entity's model
* [model](kengine/data/model.md): specifies a resource file that the entity represents
* [name](kengine/data/name.md): provides the entity's name
* [selected](kengine/data/selected.md): marks the entity as selected

#### Game development

* [keep_alive](kengine/data/keep_alive.md): keeps the main loop running
* [lua_state](kengine/data/lua_state.md): holds a [sol::state](https://github.com/ThePhD/sol2)
* [nav_mesh](kengine/data/nav_mesh.md): marks the entity as a navigable environment
* [pathfinding](kengine/data/pathfinding.md): moves the entity to a destination
* [python_state](kengine/data/python_state.md): holds the persistent Python state
* [time_modulator](kengine/data/time_modulator.md): affects the passing of time
* [transform](kengine/data/transform.md): defines the entity's position, size and rotation

#### Behaviors

* [collision](kengine/data/collision.md): called when the entity collides with another
* [input](kengine/data/input.md): receives keyboard and mouse events
* [input_buffer](kengine/data/input_buffer.md): buffers keyboard and mouse events, received by graphics systems, until they're dispatched by the input system
* [lua](kengine/data/lua.md): lists Lua scripts to execute for the entity
* [lua_table](kengine/data/lua_table.md): holds a [sol::table](https://github.com/ThePhD/sol2). Lua scripts can use it to store arbitrary information
* [python](kengine/data/python.md): lists Python scripts to execute for the entity

#### Debug tools

* [adjustable](kengine/data/adjustable.md): lets users modify variables through a GUI (such as the [imgui_adjustable](kengine/systems/imgui_adjustable/imgui_adjustable.md) system)
* [imgui_context](kengine/data/imgui_context.md): holds an ImGui context
* [imgui_scale](kengine/data/imgui_scale.md): custom scale to be applied to all ImGui elements
* [imgui_tool](kengine/data/imgui_tool.md): marks the entity as an ImGui tool, which can be enabled or disabled through the window's main menu bar

#### Graphics

* [camera](kengine/data/camera.md): turns the entity into an in-game camera, associated to a viewport
* [debug_graphics](kengine/data/debug_graphics.md): draws debug information (such as lines, boxes or spheres)
* [glfw_window](kengine/data/glfw_window.md): provides parameters to create a GLFW window
* [graphics](kengine/data/graphics.md): specifies the appearance of the entity
* [highlight](kengine/data/highlight.md): indicates that the entity should be highlighted
* [light](kengine/data/light.md): turns the entity into an in-game light
* [no_shadow](kengine/data/no_shadow.md): disables shadow casting for the entity
* [sprite](kengine/data/sprite.md): marks the entity as a 2D or 3D sprite
* [text](kengine/data/text.md): draws 2D or 3D text
* [viewport](kengine/data/viewport.md): specifies the screen area for the "camera entity"
* [window](kengine/data/window.md): turns the entity into an operating system window

#### 3D Graphics

* [god_rays](kengine/data/god_rays.md): generates volumetric lighting (also known as "Almighty God Rays") for the "light entity"
* [model_data](kengine/data/model_data.md): holds the loaded vertices and indices for a 3D model
* [polyvox](kengine/data/polyvox.md): generates a voxel-based model using the PolyVox library
* [sky_box](kengine/data/sky_box.md): draws a skybox

#### Animation

* [animation](kengine/data/animation.md): plays an animation on the entity
* [animation_files](kengine/data/animation_files.md): lists the animation files to load for the "model entity"
* [model_animation](kengine/data/model_animation.md): lists all the animations loaded for the "model entity"
* [model_skeleton](kengine/data/model_skeleton.md): describes the "model entity"'s skeleton
* [skeleton](kengine/data/skeleton.md): holds the entity's bones (used for animation and physics)

#### Physics

* [kinematic](kengine/data/kinematic.md): marks the entity as kinematic, i.e. "hand-moved" and not managed by physics systems
* [model_collider](kengine/data/model_collider.md): lists the colliders attached to the "model entity"'s skeleton
* [physics](kengine/data/physics.md): defines an entity's movement

### [Function components](kengine/functions/)

* [appears_in_viewport](kengine/functions/appears_in_viewport.md): returns whether the entity should appear in a "viewport entity"
* [execute](kengine/functions/execute.md): called each frame
* [get_entity_in_pixel](kengine/functions/get_entity_in_pixel.md): returns the entity seen in a given pixel
* [get_position_in_pixel](kengine/functions/get_position_in_pixel.md): returns the position seen in a given pixel
* [log](kengine/functions/log.md): logs a message
* [on_click](kengine/functions/on_click.md): called when the entity is clicked
* [on_collision](kengine/functions/on_collision.md): called whenever two entities collide
* [on_mouse_captured](kengine/functions/on_mouse_captured.md): indicates whether the mouse should be captured by the window
* [query_position](kengine/functions/query_position.md): returns a list of entities found within a certain distance of a position
* [register_types](kengine/functions/register_types.md): registers types with the engine's reflection utilities

### [Meta components](kengine/meta/)

In all following descriptions, the "parent component" refers to the component type represented by the type entity which has the meta component.

* [attach_to](kengine/meta/attach_to.md): attaches the parent component to an entity
* [attributes](kengine/meta/attributes.md): recursively lists the parent component's attributes
* [copy](kengine/meta/copy.md): copies the parent component from one entity to another
* [count](kengine/meta/count.md): counts the number of entities with the parent component
* [detach_from](kengine/meta/detach_from.md): detaches the parent component from an entity
* [display_imgui](kengine/meta/display_imgui.md): displays the parent component attached to an entity in ImGui with read-only attributes
* [edit_imgui](kengine/meta/edit_imgui.md): displays the parent component attached to an entity in ImGui and lets users edit attributes
* [for_each_entity](kengine/meta/for_each_entity.md): iterates on all entities with the parent component
* [for_each_entity_without](kengine/meta/for_each_entity.md): iterates on all entities without the parent component
* [get](kengine/meta/get.md): returns a pointer to the parent component attached to an entity
* [has](kengine/meta/has.md): returns whether an entity has the parent component
* [load_from_json](kengine/meta/load_from_json.md): initializes the parent component attached to an entity from a [nlohmann::json](https://github.com/nlohmann/json) object
* [match_string](kengine/meta/match_string.md): returns whether the parent component attached to an entity matches a given string
* [move](kengine/meta/move.md): moves the parent component from one entity to another
* [save_to_json](kengine/meta/save_to_json.md): serializes the parent component into a JSON object
* [size](kengine/meta/size.md): contains the size of the parent component

## [Systems](kengine/systems/)

### Behaviors
* [lua](kengine/systems/lua/lua.md): executes Lua scripts attached to entities
* [python](kengine/systems/python/python.md): executes Python scripts attached to entities
* [collision](kengine/systems/collision/collision.md): forwards collision notifications to entities
* [on_click](kengine/systems/on_click/on_click.md): forwards click notifications to entities
* [input](kengine/systems/input/input.md): forwards input events buffered by graphics systems to entities

### Debug tools
* [imgui_adjustable](kengine/systems/imgui_adjustable/imgui_adjustable.md): displays an ImGui window to edit [adjustables](kengine/data/adjustable.md)
* [imgui_engine_stats](kengine/systems/imgui_engine_stats/imgui_engine_stats.md): displays an ImGui window with engine stats
* [imgui_entity_editor](kengine/systems/imgui_entity_editor/imgui_entity_editor.md): displays ImGui windows to edit [selected](kengine/data/selected.md) entities
* [imgui_entity_selector](kengine/systems/imgui_entity_selector/imgui_entity_selector.md): displays an ImGui window to search for and select entities
* [imgui_prompt](kengine/systems/imgui_prompt/imgui_prompt.md): displays an ImGui window to run arbitrary code in Lua and Python
* [imgui_tool](kengine/systems/imgui_tool/imgui_tool.md): manages ImGui [tool windows](kengine/data/imgui_tool.md) through ImGui's MainMenuBar

### Logging
* [log_file](kengine/systems/log_file/log_file.md): outputs logs to a file
* [log_imgui](kengine/systems/log_imgui/log_imgui.md): outputs logs to an ImGui window
* [log_stdout](kengine/systems/log_stdout/log_stdout.md): outputs logs to stdout
* [log_visual_studio](kengine/systems/log_visual_studio/log_visual_studio.md): outputs logs to Visual Studio's output window

### 2D Graphics
* [sfml](kengine/systems/sfml/sfml.md): displays entities in an SFML render window
    ⬆ ⚠ Doesn't compile on MinGW in Release ⚠ ⬆

### 3D Graphics
* [glfw](kengine/systems/glfw/glfw.md): creates GLFW windows and handles their input
* [kreogl](kengine/systems/kreogl/kreogl.md): displays entities in [kreogl](https://github.com/phisko/kreogl)
* [magica_voxel](kengine/systems/polyvox/magica_voxel.md): loads 3D models in the MagicaVoxel ".vox" format
* [polyvox](kengine/systems/polyvox/polyvox.md): generates 3D models based on [polyvox components](kengine/data/polyvox.md)

### Physics
* [bullet](kengine/systems/bullet/bullet.md): simulates physics using Bullet Physics
* [kinematic](kengine/systems/kinematic/kinematic.md): moves kinematic entities

### General
* [recast](kengine/systems/recast/recast.md): generates navmeshes and performs pathfinding using Recast/Detour
* [model_creator](kengine/systems/model_creator/model_creator.md): handles [model Entities](kengine/data/model.md)

These systems must be enabled by setting the corresponding CMake variable to `ON` in your `CMakeLists.txt` (or via CMake's configuration options). Alternatively, you can set `KENGINE_ALL_SYSTEMS` to build them all. The variable for each system is `KENGINE_<SYSTEM_NAME>`. For instance, the variable for the `lua` system is `KENGINE_LUA`.

It is possible to test for the existence of these systems during compilation thanks to C++ define macros. These have the same name as the CMake variables, e.g.:
```cpp
#ifdef KENGINE_SCRIPTING_LUA
// The LuaSystem exists, and we can safely use the lua library
#endif
```

Some of these systems make use of [Conan](https://conan.io/) for dependency management. The necessary packages will be automatically downloaded when you run CMake, but Conan must be installed separately by running:
```
pip install conan
```

## [Helpers](kengine/helpers/)

These are helper functions and macros to factorize typical manipulations of components.

* [assert_helper](kengine/helpers/assert_helper.md): higher-level assertions
* [async_helper](kengine/helpers/async_helper.md): run async tasks
* [camera_helper](kengine/helpers/camera_helper.md): manipulate cameras
* [command_line_helper](kengine/helpers/command_line_helper.md): create and query the command-line
* [imgui_helper](kengine/helpers/imgui_helper.md): display and edit entities in ImGui
* [imgui_lua_helper](kengine/helpers/imgui_lua_helper.md): register Lua bindings for ImGui
* [instance_helper](kengine/helpers/instance_helper.md): access an [instance](kengine/data/instance.md)'s [model](kengine/data/model.md)
* [is_running](kengine/helpers/is_running.md): query and alter the engine's state
* [json_helper](kengine/helpers/json_helper.md): serialize and de-serialize entities from json
* [log_helper](kengine/helpers/log_helper.md): send log messages to all systems
* [lua_helper](kengine/helpers/lua_helper.md): register types and functions to Lua
* [main_loop](kengine/helpers/main_loop.md): run the core game loop
* [matrix_helper](kengine/helpers/matrix_helper.md): manipulate matrices
* [profiling_helper](kengine/helpers/profiling_helper.md): instrument code
* [python_helper](kengine/helpers/python_helper.md): register types and functions to Python
* [register_all_types](kengine/helpers/register_all_types.md): register all known types with a `registry`
* [register_metadata](kengine/helpers/meta/register_metadata.md): registers general metadata about a component type
* [register_meta_components](kengine/helpers/meta/register_meta_components.md): register all currently implemented meta components for a set of types
* [register_with_script_languages](kengine/helpers/meta/register_with_script_languages.md): register a set of types with all currently implemented script languages
* [register_everything](kengine/helpers/meta/register_everything.md): registers a set of types with all the engine's reflection facilities
* [script_language_helper](kengine/helpers/script_language_helper.md): shared code when registering types and functions to scripting languages
* [skeleton_helper](kengine/helpers/skeleton_helper.md): manipulate skeletons
* [sort_helper](kengine/helpers/sort_helper.md): sort entities
* [type_helper](kengine/helpers/type_helper.md): get a "singleton" entity representing a given type

### Meta component helpers

* [attribute_helper](kengine/helpers/meta/attribute_helper.md): get runtime attribute info from a "type entity"

These provide helper functions to register standard implementations for the respective meta components.

* [attach_to](kengine/helpers/meta/impl/attach_to.md)
* [copy](kengine/helpers/meta/impl/copy.md)
* [count](kengine/helpers/meta/impl/count.md)
* [detach_from](kengine/helpers/meta/impl/detach_from.md)
* [display_imgui](kengine/helpers/meta/display_imgui.md)
* [edit_imgui](kengine/helpers/meta/edit_imgui.md)
* [for_each_entity](kengine/helpers/meta/for_each_entity.md)
* [get](kengine/helpers/meta/get.md)
* [has](kengine/helpers/meta/has.md)
* [load_from_json](kengine/helpers/meta/load_from_json.md)
* [match_string](kengine/helpers/meta/match_string.md)
* [move](kengine/helpers/meta/move.md)
* [save_to_json](kengine/helpers/meta/save_to_json.md)

## Scripts

A [generate_type_registration](scripts/generate_type_registration.md) Python script is provided, which can be used to generate C++ files containing functions that will register a set of given types with the engine.

This is **absolutely not mandatory**.
