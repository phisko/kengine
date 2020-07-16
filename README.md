# Kengine 

The Koala engine is a type-safe and self-documenting implementation of an Entity-Component-System (ECS), with a focus on runtime extensibility and compile-time type safety and clarity.

![koala](koala.png)

## Table of contents

* [Classes](#classes)
* [Reflection](#reflection)
* [Components](#components)
    + [Data components](#data-components)
    + [Function components](#function-components)
    + [Meta components](#meta-components)
* [Samples](#samples)
    + [Components](#components-1)
        - [Data components](#data-components-1)
            * [General purpose gamedev](#general-purpose-gamedev)
            * [Behaviors](#behaviors)
            * [Debug tools](#debug-tools)
            * [Graphics](#graphics)
            * [3D Graphics](#3d-graphics)
            * [Skeletal animation](#skeletal-animation)
            * [Physics](#physics)
        - [Function components](#function-components-1)
        - [Meta components](#meta-components-1)
    + [Systems](#systems)
        - [Behaviors](#behaviors-1)
        - [Debug tools](#debug-tools-1)
        - [3D Graphics](#3d-graphics-1)
        - [2D Graphics](#2d-graphics)
        - [Physics](#physics-1)
    + [Helpers](#helpers)
        * [Meta component helpers](#meta-component-helpers)
* [Example](#example-1)

## Example

An example of source code is shown at the bottom of this page.

## Installation

The engine uses Git submodules, and should therefore be cloned recursively with
```
git clone https://github.com/phisko/kengine --recursive
```

Alternatively, the entire source code can be found in ZIP form in the latest release.

## C++ version

The engine requires a **C++17** compiler.

## Classes

* [Entity](Entity.md): can be used to represent anything (generally an in-game entity). Is simply a container of `Components`
* [EntityManager](EntityManager.md): manages `Entities` and `Components`

Note that there is no `Component` class. Any type can be used as a `Component`, and dynamically attached/detached to `Entities`.

Similarly, there is no `System` class to hold game logic. `Systems` are simply `Entities` with an `Execute` component. This lets users introspect `Systems` or add behavior to them (such as profiling) just like they would with any other `Entity`.

## Reflection

Many parts of the engine (such as the scripting systems or the OpenGL system) make use of `putils`' [reflection API](https://github.com/phisko/putils/blob/master/reflection.md). Most of the components in the following samples are thus defined as reflectible.

## Components

Any type can be used as a `Component`. This essentially means that you can use the engine solely as a generic container, letting you attach anything to anything.

However, the engine comes with a (fairly large) number of pre-built components that can be used to bootstrap a game, or simply as examples that you can base your own implementations upon.

These components fit into three categories:
* Data components
* Function components
* Meta components

### Data components

These are typical, data-holding components, such as a [TransformComponent](components/data/TransformComponent.md) or a [NameComponent](components/data/NameComponent.md). They provide information about an `Entity`.

Data components can sometimes hold functions: the [InputComponent](components/data/InputComponent.md) lets an `Entity` hold callbacks to be called whenever an input event occurs. The [CollisionComponent](components/data/CollisionComponent.md) lets an `Entity` be notified when it collides with another.

### Function components

These are simply holders for functors that can be attached as `Components` to `Entities`. This mechanic can be used:
* to attach behaviors to entities (for instance, the [Execute](components/functions/Execute.md) function gets called by the main loop each frame)
* to register callbacks for system-wide events (for instance, the [OnEntityCreated](components/functions/OnEntityCreated.md) function gets called whenever a new `Entity` is created)
* to provide new functionality that is implemented in a specific system (for instance, the [QueryPosition](components/functions/QueryPosition.md) function can only be implemented in a physics system)

Function components are types that inherit from [BaseFunction](components/BaseFunction.hpp), giving it the function signature as a template parameter.

To call a function component, one can use its `operator()` or its `call` function.

```cpp
// have an Entity e
e += functions::Execute{ // Attach a function
    [](float deltaTime) { std::cout << "Yay!\n"; }
};
const auto & execute = e.get<functions::Execute>(); // Get the function
execute(0.f); // Call it with its parameters
execute.call(42.f); // Alternatively
```

### Meta components

These provide a type-specific implementation of a generic function for a given `Component` type. They are attached to "type entities", i.e. `Entities` used to represent a `Component` type. These entities can be obtained by calling the `getTypeEntity<T>()` function from [typeHelper](helpers/typeHelper.md).

At their core, meta components are function components: they also inherit from [BaseFunction](components/BaseFunction.hpp) and are used the same way.

As an example, the [Has](components/meta/Has.md) meta component, attached to the type entity for `T`, takes an `Entity` as parameter and returns whether it has a `T` component.

```cpp
EntityManager em;

auto type = getTypeEntity<TransformComponent>(em); // Get the type entity
type += NameComponent{ "TransformComponent" }; // You'll typically want to provide the type name as information
type += meta::Has{ // Provide the implementation for `Has`
    [](const Entity & e) { return e.has<TransformComponent>(); }
};

auto e = em.createEntity([](Entity & e) { // Create an entity with a TransformComponent
    e += TransformComponent{};
});

// For each entity with a NameComponent and a Has meta component
for (const auto & [type, name, has] : em.getEntities<NameComponent, meta::Has>())
    if (has(e)) // if `e` has the component represented by `type`
        std::cout << e.id << " has a " << name.name << '\n';
```

## Samples

These are pre-built, extensible and pluggable elements that can be used to bootstrap a project or as inspiration for your own implementations.

### Components

#### Data components

##### General purpose gamedev
* [TransformComponent](components/data/TransformComponent.md): defines an `Entity`'s position, size and rotation
* [PhysicsComponent](components/data/PhysicsComponent.md): defines an `Entity`'s movement
* [KinematicComponent](components/data/KinematicComponent.md): marks an `Entity` as kinematic, i.e. "hand-moved" and not managed by physics systems
* [InputComponent](components/data/InputComponent.md): lets `Entities` receive keyboard and mouse events
* [SelectedComponent](components/data/SelectedComponent.md): indicates that an `Entity` has been selected
* [NameComponent](components/data/NameComponent.md): provides an `Entity`'s name

##### Behaviors
* [LuaComponent](components/data/LuaComponent.md): defines the lua scripts to be run by the `LuaSystem` for an `Entity`
* [LuaTableComponent](components/data/LuaTableComponent.md): holds a [sol::table](https://github.com/ThePhD/sol2) that lua scripts can use to hold any information related to an `Entity`
* [PythonComponent](components/data/PythonComponent.md): defines the Python scripts to be run by the `PythonSystem` for an `Entity`
* [CollisionComponent](components/data/CollisionComponent.md): defines a function to be called when an `Entity` collides with another
* [OnClickComponent](components/data/OnClickComponent.md): defines a function to be called when an `Entity` is clicked

##### Debug tools
* [AdjustableComponent](components/data/AdjustableComponent.md): lets users modify variables through a GUI (such as the [ImGuiAdjustableSystem](systems/imgui_adjustable/ImGuiAdjustableSystem.md))
* [ImGuiComponent](components/data/ImGuiComponent.md): lets `Entities` render debug elements using [ImGui](https://github.com/ocornut/imgui/)
* [ImGuiToolComponent](components/data/ImGuiToolComponent.md): indicates that an `Entity`'s `ImGuiComponent` is a tool that can be enabled or disabled by the [ImGuiToolSystem](systems/imgui_tool/ImGuiToolSystem.md)
* [DebugGraphicsComponent](components/data/DebugGraphicsComponent.md): lets an `Entity` be used to draw debug information (such as lines, rectangles or spheres)

##### Graphics
* [GraphicsComponent](components/data/GraphicsComponent.md): specifies the appearance of an `Entity`
* [ModelComponent](components/data/ModelComponent.md): describes a model file (be it a 3D model, a 2D sprite or any other graphical asset)
* [CameraComponent](components/data/CameraComponent.md): lets `Entities` be used as in-game cameras, to define a frustum
* [ViewportComponent](components/data/ViewportComponent.md): specifies the screen area for a "camera entity"
* [WindowComponent](components/data/WindowComponent.md): lets `Entities` be used as windows

##### 3D Graphics
* [HighlightComponent](components/data/HighlightComponent.md): indicates that an `Entity` should be highlighted
* [LightComponent](components/data/LightComponent.md): lets `Entities` be used as in-game light sources (directional lights, point lights or spot lights)
* [GodRaysComponent](components/data/GodRaysComponent.md): indicates that a "light entity" should generate volumetric lighting (also known as "Almighty God Rays")
* [ShaderComponent](components/data/ShaderComponent.md): lets `Entities` be used to introduce new OpenGL shaders
* [PolyVoxComponent](components/data/PolyVoxComponent.md): lets `Entities` be used to generate voxel-based models, drawn by the [PolyVoxSystem](systems/polyvox/PolyVoxSystem.md)
* [SkyBoxComponent](components/data/SkyBoxComponent.md): lets `Entities` be used to draw a skybox
* [SpriteComponent](components/data/SpriteComponent.md): indicates that an `Entity`'s `GraphicsComponent` describes a 2D or 3D sprite
* [TextComponent](components/data/TextComponent.md): indicates that an `Entity`'s `GraphicsComponent` describes a 2D or 3D text element

##### Skeletal animation
* [AnimationComponent](components/data/AnimationComponent.md): provides skeletal animation information for `Entities`.
* [SkeletonComponent](components/data/SkeletonComponent.md): provides bone information for an `Entity`'s skeletal animation

##### Physics
* [ModelColliderComponent](components/data/ModelColliderComponent.md): attached to an `Entity` with a [ModelComponent](components/data/ModelComponent.md). Describes the colliders associated with a given model.

#### Function components

* [Execute](components/functions/Execute.md): called each frame
* [OnEntityCreated](components/functions/OnEntityCreated.md): called for each new `Entity`
* [OnEntityRemoved](components/functions/OnEntityRemoved.md): called whenever an `Entity` is removed
* [OnTerminate](components/functions/OnTerminate.md): called during `EntityManager` destruction
* [GetEntityInPixel](components/functions/GetEntityInPixel.md): returns the `Entity` seen in a given pixel
* [GetImGuiScale](components/functions/GetImGuiScale.md): returns the scale to apply to ImGui widgets
* [OnCollision](components/functions/OnCollision.md): called whenever two `Entities` collide
* [OnMouseCaptured](components/functions/OnMouseCaptured.md): indicates whether the mouse should be captured by the window
* [QueryPosition](components/functions/QueryPosition.md): returns a list of `Entities` found within a certain distance of a position

#### Meta components

In all following descriptions, the "parent" `Component` refers to the `Component` type represented by the type entity which has the meta component.

* [Has](components/meta/Has.md): returns whether an `Entity` has the parent `Component`
* [AttachTo](components/meta/AttachTo.md): attaches the parent `Component` to an `Entity`
* [DetachFrom](components/meta/DetachFrom.md): detaches the parent `Component` from an `Entity`
* [ForEachEntity](components/meta/ForEachEntity.md): iterates on all entities with the parent `Component`
* [ForEachEntityWithout](components/meta/ForEachEntity.md): iterates on all entities without the parent `Component`
* [EditImGui](components/meta/EditImGui.md): displays the parent `Component` attached to an `Entity` in ImGui and lets users edit attributes
* [DisplayImGui](components/meta/DisplayImGui.md): displays the parent `Component` attached to an `Entity` in ImGui with read-only attributes
* [LoadFromJSON](components/meta/LoadFromJSON.md): initializes the parent `Component` attached to an `Entity` from a [putils::json](https://github.com/nlohmann/json) object
* [MatchString](components/meta/MatchString.md): returns whether the parent `Component` attached to an `Entity` matches a given string

### Systems

#### Behaviors
* [LuaSystem](systems/lua/LuaSystem.md): executes lua scripts attached to `Entities`
* [PythonSystem](systems/python/PythonSystem.md): executes Python scripts attached to `Entities`
* [CollisionSystem](systems/collision/CollisionSystem.md): forwards collision notifications to `Entities`
* [OnClickSystem](systems/onclick/OnClickSystem.md): forwards click notifications to `Entities`
* [InputSystem](systems/input/InputSystem.md): forwards input events buffered by graphics systems to `Entities`

#### Debug tools
* [ImGuiAdjustableSystem](systems/imgui_adjustable/ImGuiAdjustableSystem.md): displays an ImGui window to edit `AdjustableComponents`
* [ImGuiEntityEditorSystem](systems/imgui_entity_editor/ImGuiEntityEditorSystem.md): displays ImGui windows to edit `Entities` with a `SelectedComponent`
* [ImGuiEntitySelectorSystem](systems/imgui_entity_selector/ImGuiEntitySelectorSystem.md): displays an ImGui window that lets users search for and select `Entities`
* [ImGuiToolSystem](systems/imgui_tool/ImGuiToolSystem.md): manages ImGui [tool windows](components/data/ImGuiToolComponent.md) through ImGui's MainMenuBar

#### 3D Graphics
* [OpenGLSystem](systems/opengl/OpenGLSystem.md): displays entities in an OpenGL render window
* [OpenGLSpritesSystem](systems/opengl_sprites/OpenGLSpritesSystem.md): loads sprites and provides shaders to render them 
* [AssimpSystem](systems/assimp/AssimpSystem.md): loads 3D models using the assimp library, animates them and provides shaders to render them
* [PolyVoxSystem](systems/polyvox/PolyVoxSystem.md): generates 3D models based on `PolyVoxComponents` and provides shaders to render them
* [MagicaVoxelSystem](systems/polyvox/MagicaVoxelSystem.md): loads 3D models in the MagicaVoxel ".vox" format, which can then be drawn by the `PolyVoxSystem`'s shader

#### Physics
* [BulletSystem](systems/bullet/BulletSystem.md): simulates physics using Bullet Physics
* [KinematicSystem](systems/kinematic/KinematicSystem.md): moves kinematic `Entities`

These systems must be enabled by setting the corresponding CMake variable to `true` in your `CMakeLists.txt`. Alternatively, you can set `KENGINE_ALL_SYSTEMS` to build them all.

| System                    | Variable                      |
|---------------------------|-------------------------------|
| AssimpSystem              | KENGINE_ASSIMP                |
| BulletSystem              | KENGINE_BULLET                |
| CollisionSytem            | KENGINE_COLLISION             |
| ImGuiAdjustableSystem     | KENGINE_IMGUI_ADJUSTABLE      |
| ImGuiEntityEditorSystem   | KENGINE_IMGUI_ENTITY_EDITOR   |
| ImGuiEntitySelectorSystem | KENGINE_IMGUI_ENTITY_SELECTOR |
| ImGuiPromptSystem         | KENGINE_IMGUI_PROMPT          |
| ImGuiToolSystem           | KENGINE_IMGUI_TOOL            |
| InputSystem               | KENGINE_INPUT                 |
| KinematicSystem           | KENGINE_KINEMATIC             |
| LuaSystem                 | KENGINE_LUA                   |
| OnClickSystem             | KENGINE_ONCLICK               |
| OpenGLSystem              | KENGINE_OPENGL                |
| OpenGLSpritesSystem       | KENGINE_OPENGL_SPRITES        |
| PolyVoxSystem             | KENGINE_POLYVOX               |
| MagicaVoxelSystem         | KENGINE_POLYVOX               |
| PythonSystem              | KENGINE_PYTHON                |
| RecastSystem              | KENGINE_RECAST                |

It is possible to test for the existence of these systems during compilation thanks to C++ define macros. These have the same name as the CMake variables, e.g.:
```cpp
#ifdef KENGINE_LUA
// The LuaSystem exists, and we can safely use the lua library
#endif
```

Some of these systems make use of [Conan](https://conan.io/) for dependency management. The necessary packages will be automatically downloaded when you run CMake, but Conan must be installed separately by running:
```
pip install conan
```

### Helpers

These are helper functions to factorize typical manipulations of `Components`.

* [cameraHelper](helpers/cameraHelper.md)
* [imguiHelper](helpers/imguiHelper.md): provides helpers to display and edit `Entities` in ImGui
* [mainLoop](helpers/mainLoop.md)
* [matrixHelper](helpers/matrixHelper.md)
* [pluginHelper](helpers/pluginHelper.md): provides an `initPlugin` function to be called from DLLs
* [shaderHelper](systems/opengl/shaderHelper.md)
* [skeletonHelper](helpers/skeletonHelper.md)
* [sortHelper](helpers/sortHelper.md): provides functions to sort `Entities`
* [typeHelper](helpers/typeHelper.md): provides a `getTypeEntity<T>` function to get a "singleton" entity representing a given type

##### Meta component helpers

* [registerComponentFunctions](helpers/meta/registerComponentFunctions.md): provides implementations for the [Has](components/meta/Has.md), [AttachTo](components/meta/AttachTo.md) and [DetachFrom](components/meta/DetachFrom.md) meta components
* [registerComponentEntityIterators](helpers/meta/registerComponentEntityIterators.md): provides implementations for the [ForEachEntity](components/meta/ForEachEntity.md) and [ForEachEntityWithout](components/meta/ForEachEntity.md) meta components
* [registerComponentJSONLoader](helpers/meta/registerComponentJSONLoader.md): provides an implementation for the [LoadFromJSON](components/meta/LoadFromJSON.md)
* [registerComponentEditor](helpers/meta/registerComponentEditor.md): provides implementations for the [EditImGui](components/meta/ImGuiEditor.md) and [DisplayImGui](components/meta/ImGuiEditor.md) meta components
* [registerComponentMatcher](helpers/meta/registerComponentMatcher.md): provides an implementation for the [MatchString](components/meta/MatchString.md) meta component

## Example

Below is a commented main function that creates an entity and attaches some components to it, as well as a lua script. This should let you get an idea of what is possible using the kengine's support for reflection and runtime extensibility, as well as the compile-time clarity and type-safety that were the two motivations behind the project.

### main.cpp

```cpp
#include <iostream>

#include "go_to_bin_dir.hpp"

#include "EntityManager.hpp"
#include "Entity.hpp"

#include "systems/lua/LuaSystem.hpp"

#include "data/GraphicsComponent.hpp"
#include "data/TransformComponent.hpp"
#include "functions/Execute.hpp"

#include "helpers/mainLoop.hpp"
#include "helpers/luaHelper.hpp"

// Simple system that outputs the transform and lua components of each entity that has them
//- Forward declaration
static float execute(kengine::EntityManager & em, float deltaTime);
//-
auto DebugSystem(kengine::EntityManager & em) {
    return [&](kengine::Entity & e) {
        // Attach an Execute component that will be called each frame
        e += kengine::functions::Execute{
             [&](float deltaTime) { execute(em, deltaTime); }
        };
    };
}

// This could be defined as a lambda in DebugSystem but is moved out here for readability
static float execute(kengine::EntityManager & em, float deltaTime) {
    for (const auto & [e, transform, lua] : em.getEntities<kengine::TransformComponent, kengine::LuaComponent>()) {
        std::cout << "Entity " << e.id << '\n';
        std::cout << "\tTransform: "
            << transform.boundingBox.position.x << ' '
            << transform.boundingBox.position.y << ' '
            << transform.boundingBox.position.z << '\n';

        std::cout << "\tScripts:" << '\n';
        for (const auto & script : lua.scripts)
            std::cout << "\t\t[" << script << "]\n";

        std::cout << '\n';
    }
}

int main(int, char **av) {
    // Go to the executable's directory to be next to resources and scripts
    putils::goToBinDir(av[0]);

    // Create an EntityManager
    kengine::EntityManager em; // Optionally, pass a number of threads as parameter -> kengine::EntityManager em(4);

    em += DebugSystem(em);
    em += kengine::LuaSystem(em);

    // Create an Entity and attach Components to it
    em += [](kengine::Entity e) {
        e += kengine::TransformComponent({ 42.f, 0.f, 42.f }); // Parameter is a Point3f for position
        e += kengine::LuaComponent({ "scripts/unit.lua" }); // Parameter is a vector of scripts
    };

	// Register types to be used in lua
    kengine::luaHelper::registerTypes<
        kengine::TransformComponent, putils::Point3f, putils::Rect3f,
        kengine::LuaComponent
    >(em);

    // Start game
    kengine::MainLoop::run(em);

    return 0;
}
```

### scripts/unit.lua

```lua
-- Simply modify component

local transform = self:getTransformComponent()
local pos = transform.boundingBox.position
pos.x = pos.x + 1
```
