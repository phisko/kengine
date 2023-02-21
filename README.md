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

The engine code is organized in three categories:
* components: hold data **or functions** (see [Component categories](#component-categories) below)
* systems: entities that implement an engine feature
* helpers: functions to simplify component manipulation

Note that `systems` aren't objects of a specific class. Systems are simply entities with an [execute](kengine/main_loop/functions/execute.md) component (or anything else they need to do their work). The entity then lives in the `registry` with the rest of the game state. This lets users introspect systems or add behavior to them just like any other entity.

These three categories are split into various libraries, e.g.:
* [core](kengine/core/)
* [pathfinding](kengine/pathfinding/)
* [render](kengine/render/)
* ...

Note that some libraries contain sub-libraries, e.g.:
* [imgui/entity_editor](kengine/imgui/entity_editor/)
* [pathfinding/recast](kengine/pathfinding/recast/)
* [render/sfml](kengine/render/sfml/)
* ...

The [CMake](#cmake) section goes into more detail of how to work with these libraries.

## Component categories

The engine comes with a (fairly large) number of pre-built components that can be used to bootstrap a game, or simply as examples that you can base your own implementations upon.

These components fit into three categories:
* [data components](#data-components)
* [function components](#function-components)
* [meta components](#meta-components)

### Data components

Data components hold data about their entity.

Data components are what first comes to mind when thinking of a component, such as a [transform](kengine/core/data/transform.md) or a [name](kengine/core/data/name.md).

Data components can sometimes hold functions:
* [input](kengine/input/data/input.md) lets an entity hold callbacks to be called whenever an input event occurs
* [collision](kengine/physics/collision/data/collision.md) lets an entity be notified when it collides with another

### Function components

Function components hold functions to query, alter, or notify their entity.

Function components are simply holders for functors that can be attached as components to entities. This mechanic can be used to:
* attach behaviors to entities: [execute](kengine/main_loop/functions/execute.md) is called by the main loop each frame
* register callbacks for system-wide events: [on_click](kengine/render/on_click/functions/on_click.md) is called whenever the user clicks the entity
* provide new functionality that is implemented in a specific system: [query_position](kengine/physics/functions/query_position.md) is typically implemented by a physics system

Function components are types that inherit from [base_function](kengine/base_function.hpp), giving it the function signature as a template parameter.

To call a function component, one can use its `operator()` or its `call` function.

```cpp
entt::registry r;
const auto e = r.create();
r.emplace<functions::execute>(e,
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

An example makes this clearer: [edit_imgui](kengine/meta/functions/edit_imgui.md) is a meta component that, when called, will draw its "parent component"'s properties using [ImGui](https://github.com/ocornut/imgui/) for the given entity. The following code will display a window to edit `e`'s [name](kengine/core/data/name.md) component.

```cpp
// r is a registry with the "type entity" for `name` already setup

const auto e = r.create();
r.emplace<core::name>(e);

const auto type_entity = type_helper::get_type_entity<core::name>(r);
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

## Libraries

See [CMake](#cmake) for instructions on how to enable each library.

* [kengine_core](kengine/core/): components and helpers that are accessed by most (if not all) other libraries
    * [kengine_core_assert](kengine/core/assert/): engine-level assertions
    * [kengine_core_log](kengine/core/log/): generic logging support
        * [kengine_core_log_file](kengine/core/log/file/): log to a file
        * [kengine_core_log_imgui](kengine/core/log/imgui/): log to an ImGui window
        * [kengine_core_log_standard_output](kengine/core/log/standard_output/): log stdout
        * [kengine_core_log_visual_studio](kengine/core/log/visual_studio/): log to VS's output window
    * [kengine_core_profiling](kengine/core/profiling/): profiling using [Tracy](https://github.com/wolfpld/tracy)
    * [kengine_core_sort](kengine/core/sort/): entity sorting helpers
* [kengine_adjustable](kengine/adjustable/): expose global values that the user may adjust
    * [kengine_adjustable_imgui](kengine/adjustable/imgui/): display adjustables in an ImGui window
* [kengine_async](kengine/async/): run asynchronous tasks
    * [kengine_async_imgui](kengine/async/imgui/): display running tasks in an ImGui window
* [kengine_command_line](kengine/command_line/): manipulate the command-line
* [kengine_glm](kengine/glm/): use [GLM](https://github.com/g-truc/glm)
* [kengine_imgui](kengine/imgui/): use [ImGui](https://github.com/ocornut/imgui)
    * [kengine_imgui_tool](kengine/imgui/tool/): enable/disable ImGui tools from the main menu bar
* [kengine_input](kengine/input/): handle user input
* [kengine_json_scene_loader](kengine/json_scene_loader/): load scenes from JSON files
* [kengine_main_loop](kengine/main_loop/): run a game's main loop, handling delta time
* [kengine_meta](kengine/meta/): meta components and reflection facilities
    * [kengine_meta_imgui](kengine/meta/imgui/): meta components for ImGui
        * [kengine_meta_imgui_engine_stats](kengine/imgui/engine_stats/): display engine stats in an ImGui window
        * [kengine_meta_imgui_entity_editor](kengine/imgui/entity_editor/): edit entities in ImGui windows
        * [kengine_meta_imgui_entity_selector](kengine/imgui/entity_selector/): select entities in an ImGui window
    * [kengine_meta_json](kengine/meta/json/): meta components for JSON
* [kengine_model_instance](kengine/model_instance/): use model entities, which other entities can be instances of
* [kengine_pathfinding](kengine/pathfinding/): add pathfinding capabilities to entities
    * [kengine_pathfinding_recast](kengine/pathfinding/recast/): implement pathfinding using [Recast](https://github.com/recastnavigation/recastnavigation)
* [kengine_physics](kengine/physics/): move and query entities in space
    * [kengine_physics_bullet](kengine/physics/bullet/): implement physics using [Bullet](https://github.com/bulletphysics/bullet3)
* [kengine_render](kengine/render/): display entities in graphical applications
    * [kengine_render_animation](kengine/animation/): play animations on entities
    * [kengine_render_kreogl](kengine/render/kreogl/): implement rendering using [kreogl](https://github.com/phisko/kreogl)
    * [kengine_render_on_click](kengine/render/on_click/): notify entities that are clicked by the user
    * [kengine_render_sfml](kengine/render/sfml/): implement rendering using [SFML](https://github.com/SFML/SFML)
* [kengine_scripting](kengine/scripting/): run scripts from other languages
    * [kengine_scripting_imgui_prompt](kengine/scripting/imgui_prompt/): interpret scripting commands in an ImGui window
    * [kengine_scripting_lua](kengine/scripting/lua/): run scripts in Lua
        * [kengine_scripting_lua_imgui_bindings](kengine/scripting/lua/imgui_bindings/): ImGui bindings for Lua
    * [kengine_scripting_python](kengine/scripting/python/): run scripts in Python
* [kengine_skeleton](kengine/skeleton/): manipulate entities' skeletons
* [kengine_system_creator](kengine/system_creator/): helpers to manipulate system entities

## Scripts

A [generate_type_registration](scripts/generate_type_registration.md) Python script is provided, which can be used to generate C++ files containing functions that will register a set of given types with the engine.

This is **absolutely not mandatory**.

## CMake

The engine uses [CMake](cmake.org/) as a build system. A custom framework has been put in place to simplify the creation of libraries. The [root CMakeLists](CMakeLists.txt) iterates over sub-directories and automatically adds them as libraries if they match a few conditions.

A base `kengine` *interface library* is created that links against all enabled libraries, so clients may simply link against that.

### Options

The following CMake options are exposed.

#### `KENGINE_TESTS`

Compiles test executables for the libraries that implement tests.

#### `KENGINE_NDEBUG`

Disables debug code.

#### `KENGINE_TYPE_REGISTRATION`

Will generate [type registration](scripts/generate_type_registration.md) code for engine types. This is central to many of the engine's reflection capabilities, as it provides the implementation for [meta components](#meta-components).

#### `KENGINE_GENERATE_REFLECTION`

Will update the [reflection headers](https://github.com/phisko/reflection/blob/main/scripts/generate_reflection_headers.md) for engine types. These are pre-generated, so unless you're modifying the engine's source code you shouldn't need to enable this.

#### Libraries

All libraries are disabled by default, to avoid building unwanted dependencies. Each library can be enabled individually by setting its CMake option to `ON`. See [Library naming](#library-naming) for the option name.

Alternatively, all libraries can be enabled with the `KENGINE_ALL_SYSTEMS` option.

Note that sub-libraries need their parent library to be enabled: [kengine_imgui_entity_editor](kengine/imgui/entity_editor/) requires [kengine_imgui](kengine/imgui/).

### Library naming

Libraries are named depending on their relative path to the engine root. The slashes in the path are simply replaced by underscores, e.g.:
* [kengine/core](kengine/core/): `kengine_core`
* [kengine/imgui/tool](kengine/imgui/tool/): `kengine_imgui_tool`

These names are:
* used when linking against a specific library
* used for the CMake option to enable a library (e.g. `KENGINE_CORE` for `kengine_core`)
* used for a library's internal [export macro](https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html) (e.g. `KENGINE_CORE_EXPORT` for `kengine_core`)

It is possible to test for the existence of a library during compilation thanks to C++ define macros. These have the same name as the CMake options, e.g.:
```cpp
#ifdef KENGINE_CORE
// The kengine_core library exists
#endif
```

### Third-party dependencies

Some libraries make use of [Conan](https://conan.io/) for dependency management. The necessary packages will be automatically downloaded when you run CMake.

### Library creation

Since libraries are automatically detected by the root `CMakeLists.txt`, creating a new library is fairly easy.

Libraries automatically link against `kengine_core`, since it provides helpers that should be used by all libraries (such as the [log_helper](kengine/core/helpers/log_helper.md) and the [profiling_helper](kengine/core/helpers/profiling_helper.md)).

Sub-libraries automatically link against their parent. For instance, [kengine_imgui_entity_editor](kengine/imgui/entity_editor/) automatically links against [kengine_imgui](kengine/imgui/).

#### Sources

Source files from a library's `helpers` and `systems` subdirectories are automatically added. If none are found, the library will be a CMake interface library.

#### Type registration and reflection code generation

[Type registration](#kengine_type_registration) and [reflection code](#kengine_generate_reflection) may be automatically generated for components. By default, all headers in a library's `data` and `functions` subdirectories will be passed to the generation scripts.

#### [GoogleTest](https://github.com/google/googletest)

Similarly to source files, if any `*.tests.cpp` files are found in a library's `helpers/tests` or `systems/tests` subdirectories, a GoogleTest executable will be automatically added.

#### Custom `CMakeLists.txt`

Basic libraries shouldn't need their own `CMakeLists.txt`, since their source files will be automatically. However, if a library needs custom behavior (e.g. to add extra sources or to link against a third-party library), it may add its own `CMakeLists.txt`. That `CMakeLists.txt` will be called **after** the call to `add_library`.

The following variables and functions are defined before calling the `CMakeLists.txt`:
* `system_name`: the library's name
* `system_tests_name`: the library's GoogleTest target's name
* `link_type`: the library's link type (`PUBLIC` or `INTERFACE`, depending on whether sources were found or not)
* `system_link_public_libraries(libraries)`: links against other libraries (publicly)
* `system_link_private_libraries(libraries)`: links against other libraries (privately)
* `register_types_from_headers(headers)`: adds headers for which [type registration](#kengine_type_registration) and [reflection headers](#kengine_generate_reflection) may be generated
* `subdirectory_is_not_system(path)`: indicates to the root `CMakeLists.txt` that it shouldn't process `path` as a kengine library