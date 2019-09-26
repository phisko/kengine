# Kengine 

The Koala engine is a type-safe and self-documenting implementation of an Entity-Component-System (ECS), with a focus on runtime extensibility and compile-time type safety and clarity.

This new version features a greatly optimized memory architecture, better cache-efficiency, and a more advanced API.

![koala](koala.png)

## Installation

The engine uses Git submodules, and should therefore be cloned recursively with
```
git clone https://github.com/phisko/kengine --recursive
```

Alternatively, the entire source code can be found in ZIP form in the latest release.

## C++ version

The engine requires a **C++17** compiler.

## Classes

* [Entity](Entity.md): represents an in-game entity. Is simply a container of `Components`
* [System](System.md): holds game logic. A `PhysicsSystem` might control the movement of `Entities`, for instance.
* [EntityManager](EntityManager.md): manages `Entities`, `Components` and `Systems`

Note that there is no `Component` class. Any type can be used as a `Component`, and dynamically attached/detached to `Entities`.

### Reflection

Many parts of the engine (such as the scripting systems or the OpenGL system) make use of `putils`' [reflection API](https://github.com/phisko/putils/tree/master/reflection). Most of the components in the following samples are thus defined as reflectible.

### Samples

These are pre-built, extensible and pluggable elements that can be used in any project, or to bootstrap a project.

### Components

General purpose gamedev:
* [TransformComponent](common/components/TransformComponent.md): defines a `Entity`'s position and size
* [PhysicsComponent](common/components/PhysicsComponent.md): defines a `Entity`'s movement
* [InputComponent](common/components/InputComponent.md): lets `Entities` receive keyboard and mouse events
* [SelectedComponent](common/components/SelectedComponent.hpp): indicates that an `Entity` has been selected
* [NameComponent](common/components/NameComponent.hpp): provides an `Entity`'s name, e.g. for debug purposes

Behaviors:
* [BehaviorComponent](common/components/BehaviorComponent.md): defines a function to be called each frame for an `Entity`
* [LuaComponent](common/components/LuaComponent.md): defines the lua scripts to be run by the `LuaSystem` for an `Entity`
* [PyComponent](common/components/PyComponent.md): defines the Python scripts to be run by the `PySystem` for an `Entity`
* [CollisionComponent](common/components/CollisionComponent.md): defines a function to be called when an `Entity` collides with another
* [OnClickComponent](common/components/OnClickComponent.md): defines a function to be called when an `Entity` is clicked

Debug tools:
* [AdjustableComponent](common/components/AdjustableComponent.md): lets users modify variables through a GUI (such as the [ImGuiAdjustableManagerSystem](common/systems/ImGuiAdjustableSystem.md))
* [ImGuiComponent](common/components/ImGuiComponent.md): lets `Entities` render debug elements using [ImGui](https://github.com/ocornut/imgui/)
* [DebugGraphicsComponent](common/components/DebugGraphicsComponent.hpp): lets an `Entity` be used to draw debug information (such as lines, rectangles or spheres)

Graphics:
* [GraphicsComponent](common/components/GraphicsComponent.md): specifies the appearance of an `Entity`
* [ModelComponent](common/components/ModelComponent.md): describes a model file (be it a 3D model, a 2D sprite or any other graphical asset)
* [CameraComponent](common/components/CameraComponent.hpp): lets `Entities` be used as in-game cameras, to define a frustrum and position. Follows the same conventions as `TransformComponent`
* [GUIComponent](common/components/GUIComponent.md): lets `Entities` be used as GUI elements such as buttons, lists...)

3D Graphics:
* [HighlightComponent](common/components/HighlightComponent.md): indicates that an `Entity` should be highlighted
* [LightComponent](common/components/LightComponent.md): lets `Entities` be used as in-game light sources (directional lights, point lights or spot lights)
* [ShaderComponent](common/components/ShaderComponent.md): lets `Entities` be used to introduce new OpenGL shaders
* [PolyVoxComponent](common/components/PolyVoxComponent.md): lets `Entities` be used to generate voxel-based models, drawn by the `PolyVoxSystem`
* [SkyBoxComponent](common/components/SkyBoxComponent.hpp): lets `Entities` be used to draw a skybox
* [SpriteComponent](common/components/SpriteComponent.hpp): indicates that an `Entity`'s `GraphicsComponent` describes a 2D or 3D sprite
* [TextComponent](common/components/TextComponent.hpp): indicates that an `Entity`'s `GraphicsComponent` describes a 2D or 3D text

Skeletal animation:
* [AnimationComponent](common/components/AnimationComponent.md): provides skeletal animation information for `Entities`.
* [SkeletonComponent](common/components/SkeletonComponent.hpp): provides bone information for an `Entity`'s skeletal animation

Physics:
* [ModelColliderComponent](common/components/ModelColliderComponent.md): attached to an `Entity` with a [ModelComponent](common/components/ModelComponent.md). Describes the colliders associated with a given model.

### Systems

Behaviors:
* [BehaviorSystem](common/systems/BehaviorSystem.md): executes behaviors attached to `Entities`
* [LuaSystem](common/systems/LuaSystem.md): executes lua scripts attached to an entity
* [PySystem](common/systems/PySystem.md): executes Python scripts attached to an entity
* [CollisionSystem](common/systems/CollisionSystem.md): transfers collision notifications to `Entities`
* [OnClickSystem](common/systems/OnClickSystem.md): transfers click notifications to `Entities`

Debug tools:
* [ImGuiAdjustableSystem](common/systems/ImGuiAdjustableSystem.md): displays an ImGui window to edit `AdjustableComponents`
* [ImGuiEntityEditorSystem](common/systems/ImGuiEntityEditorSystem.md): displays ImGui windows to edit `Entities` with a `SelectedComponent`
* [ImGuiEntitySelectorSystem](common/systems/ImGuiEntitySelectorSystem.md): displays an ImGui window that lets users search for and select `Entities`
* [LogSystem](common/systems/LogSystem.md): logs messages

Graphics:
* [SfSystem](common/systems/sfml/SfSystem.md): displays entities in an SFML render window

3D Graphics:
* [OpenGLSystem](common/systems/opengl/OpenGLSystem.md): displays entities in an OpenGL render window
* [OpenGLSpritesSystem](common/systems/opengl_sprites/OpenGLSpritesSystem.md): loads sprites and provides shaders to render them 
* [AssimpSystem](common/systems/assimp/AssimpSystem.md): loads 3D models using the assimp library, animates them and provides shaders to render them
* [PolyVoxSystem](common/systems/polyvox/PolyVoxSystem.md): generates 3D models based on `PolyVoxComponents` and provides shaders to render them
* [MagicaVoxelSystem](common/systems/polyvox/PolyVoxSystem.md): loads 3D models in the MagicaVoxel ".vox" format, which can then be drawn by the `PolyVoxSystem`'s shader

Physics:
* [BulletSystem](common/systems/bullet/BulletSystem.md): simulates physics using Bullet Physics

Some of these systems make use of external libraries which you may not want to depend upon, and are therefore disabled by default. To enable them, set the corresponding CMake variable to `true` in your `CMakeLists.txt`:

| System         | Variable        |
|----------------|-----------------|
| SfSystem       | KENGINE_SFML    |
| ImGuiOverlaySystem | KENGINE_IMGUI_OVERLAY |
| OpenGLSystem   | KENGINE_OPENGL  |
| AssimpSystem   | KENGINE_ASSIMP  |
| PolyVoxSystem  | KENGINE_POLYVOX |
| PolyVoxSystem  | KENGINE_POLYVOX |
| MagicaVoxelSystem | KENGINE_POLYVOX |
| BulletSystem   | KENGINE_BULLET  |
| lua library    | KENGINE_LUA     |
| python library | KENGINE_PYTHON  |

These systems make use of [Conan](https://conan.io/) for dependency management. The necessary packages will be automatically downloaded when you run CMake, but Conan must be installed separately by running:
```
pip install conan
```

### DataPackets

These are datapackets sent from one `System` to another to communicate.

* [AddImGuiTool](common/packets/AddImGuiTool.hpp): received by the graphics system, adds an entry to the "Tools" section of the ImGui main menu bar
* [ImGuiScale](common/packets/ImGuiScale.hpp): sent by some graphics systems, provides a reference to a `float` that should be used to scale all ImGui elements (for special DPI screens)
* [Collision](common/packets/Collision.hpp): sent by the `BulletSystem`, indicates a collision between two `Entities`
* [QueryPosition](common/packets/Collision.hpp): handled by the `BulletSystem`, provides a list of `Entities` found within an area
* [EntityInPixel](common/packets/EntityInPixel.hpp): handled by the `OpenGLSystem`, returns the ID of the entity seen in the requested pixel
* [GBuffer](common/packets/GBuffer.hpp): received by the `OpenGLSystem`, specifies the layout of the GBuffer (used in deferred shading)
* [Log](common/packets/Log.hpp): received by the `LogSystem`, used to log a message
* [RegisterAppearance](common/packets/RegisterAppearance.hpp): received by the `SfSystem`, maps an abstract appearance to a concrete texture file.

### Helpers

* [ShaderHelper](common/systems/opengl/ShaderHelper.hpp): provides helper functions and RAII structs for OpenGL shaders
* [SkeletonHelper](common/helpers/SkeletonHelper.hpp): provides helper functions when manipulating `SkeletonComponents`
* [RegisterComponentFunctions](common/helpers/RegisterComponentFunctions.hpp): registers `ComponentFunctions` for querying an `Entity`'s components (`has`, `attach`, `detach`)
* [RegisterComponentEditor](common/helpers/RegisterComponentEditor.hpp): registers `ComponentFunctions` for displaying and editing a `Component` in ImGui 
* [RegisterComponentMatcher](common/helpers/RegisterComponentMatcher.hpp): registers a `ComponentFunction` for matching a `Component`'s attributes with a string

These are helper functions to factorize typical manipulations of `Components`.


## ComponentFunctions

```
This is an advanced feature and understanding this is absolutely not required to make use of the engine, although those of you looking to make the most of runtime extensibility and compile-time type-safety should be interested in this.
```

One of the main goals of this engine is to be as runtime-extensible as possible, while also allowing developers to perform type-based metaprogramming for things such as serialization or object inspection.

To make this possible, a "ComponentFunction" mechanism is in place. A `ComponentFunction` is a function pointer registered provided to the `EntityManager`, which matches a given `Component`. It will typically a function template's specialization for that `Component`. `ComponentFunctions` are provided by calling `registerComponentFunction`, and can then be accessed through `getComponentFunctionMaps`.

The framework is a bit hard to wrap one's head around, so here is an example of a final use case:

```cpp
// This is a `ComponentFunction` with the `void(const putils::json &, kengine::Entity &)` prototype
//                Think of this as a C++ function DECLARATION
struct LoadFromJSON : functions::BaseFunction<void(const putils::json &, kengine::Entity &)>;

EntityManager em;

// This is the implementation of the `ComponentFunction` that is going to be given to the `EntityManager`
//                Think of this as a C++ function DEFINITION
template<typename Comp>
void loaderImpl(const putils::json &, Entity & e) {
    // really complicated loader code
    e.attach<Comp>();
}

void registerComponentLoaders() {
    // provide the `EntityManager` with the JSON loading implementation for the `Components` I'll use
    // This says: for `TransformComponent3f`, people asking for `LoadFromJSON` will get `loaderImpl<TransformComponent3f>`
    em.registerComponentFunction<TransformComponent3f>(LoadFromJSON{ loaderImpl<TransformComponent3f> });
    em.registerComponentFunction<GraphicsComponent>(LoadFromJSON{ loaderImpl<GraphicsComponent> });
}

void loadEntityFromJSON(Entity & e, const putils::json & json) {
    for (const auto comp : em.getComponentFunctionMaps()) { // For each known `Component` type
        const auto loader = comp->getFunction<LoadFromJSON>(); // Try to get its `LoadFromJSON` implementation
        if (loader != nullptr) // If an implementation was provided
            loader(json, e); // I can call it!
    }
}
```

`ComponentFunctions` are identified by their type. This type must inherit from [BaseFunction](common/functions/BaseFunction.hpp), which takes as a template parameter the function signature. The type doesn't need to contain any implementation of the actual function, as it will only be used to __identify__ the function. Think of this as a C++ function __declaration__.

To register a `ComponentFunction`'s implementation for a specific type, `registerComponentFunction` must be called, taking as parameter the `ComponentFunction`, constructed with the actual function template specialization. The `EntityManager` then stores these `typeid -> function pointer` pairings in a __ComponentFunctionMap__ for each `Component`.

The `ComponentFunctionMaps` can then be retrieved by calling `getComponentFunctionMaps()`, and have a `getFunction<ComponentFunction>()` member function that will return the registered function pointer, or `nullptr`.

Example `ComponentFunctions` are provided in `common/functions`. They each come with a helper function that registers the `ComponentFunction`'s implementation.

| Name | Helper | Function |
|------|--------|----------|
| [Has, Attach, Detach](common/functions/Basic.hpp) | [registerComponentFunctions](common/helpers/RegisterComponentFunctions.hpp) | Call `Entity::has<Component>`, `Entity::attach<Component>` and `Entity::detach<Component>`|
| [LoadFromJSON](common/functions/LoadFromJSON.hpp) | [registerComponentJSONLoader](common/helpers/RegisterComponentJSONLoader.hpp) | Unserializes the `Component` from JSON and adds it to the `Entity` |
| [MatchString](common/functions/MatchString.hpp) | [registerComponentMatcher](common/helpers/RegisterComponentMatcher.hpp) | Returns whether any of the `Component`'s attributes matches a given string |
| [ImGuiEditor](common/functions/ImGuiEditor.hpp) | [registerComponentEditor](common/helpers/RegisterComponentEditor.hpp) | Displays the `Component` using ImGui |

If you wish to see concrete examples of this system in use, take a look at the [ImGuiEntityEditorSystem](common/systems/ImGuiEntityEditorSystem.hpp) or the [ImGuiEntitySelectorSystem](common/systems/ImGuiEntitySelectorSystem.hpp).

## Example

Below is a commented main function that creates an entity and attaches some components to it, as well as a lua script. This should let you get an idea of what is possible using the kengine's support for reflection and runtime extensibility, as well as the compile-time clarity and type-safety that were the two motivations behind the project.

##### main.cpp

```cpp
#include <iostream>

#include "go_to_bin_dir.hpp"

#include "EntityManager.hpp"
#include "Entity.hpp"

#include "systems/LuaSystem.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/TransformComponent.hpp"

// Simple system that outputs the transform and lua components of each entity that has them
class DebugSystem : public kengine::System<DebugSystem> {
public:
    DebugSystem(kengine::EntityManager & em) : System<DebugSystem>(em), _em(em) {}

    void execute() final {
        for (const auto & [e, transform, lua] : _em.getEntities<kengine::TransformComponent3f, kengine::LuaComponent>()) {
            std::cout << "Entity " << e.id << '\n';

            std::cout << "\tTransform: " << transform.boundingBox << '\n';

            std::cout << "\tScripts:" << '\n';
            for (const auto & script : lua.getScripts())
                std::cout << "\t\t[" << script << "]\n";

            std::cout << '\n';
        }
    }

private:
	kengine::EntityManager & _em;
};

int main(int, char **av) {
    // Go to the executable's directory to be next to resources and scripts
    putils::goToBinDir(av[0]);

    // Create an EntityManager
    kengine::EntityManager em; // Optionally, pass a number of threads as parameter (kengine::EntityManager em(4);)

    // Load the specified systems, and any plugin placed in the executable's directory
    //      If you specify 'KENGINE_SFML' as TRUE in your CMakeLists.txt, this will load the SfSystem
    em.loadSystems<DebugSystem, kengine::LuaSystem>(".");

    // To add a new system, simply add a DLL with a
    //      `ISystem *getSystem(kengine::EntityManager &em)`
    // To add a new system, simply add a DLL with a

    // Create an Entity and attach Components to it
    em += [](kengine::Entity e) {
        e += kengine::TransformComponent3f({ 42.f, 0.f, 42.f }); // Parameter is a Point3f for position
        e += kengine::LuaComponent({ "scripts/unit.lua" }); // Parameter is a vector of scripts
    };

	// Register types to be used in lua
    try {
        auto &lua = em.getSystem<kengine::LuaSystem>();
        lua.registerTypes<
            kengine::TransformComponent3f, putils::Point<float, 3>, putils::Rect<float, 3>,
            kengine::LuaComponent
        >();
    }
    catch (const std::out_of_range &) {} // If the LuaSystem wasn't found, ignore

    // Start game
    while (em.running)
        em.execute();

    return (EXIT_SUCCESS);
}
```

##### scripts/unit.lua

```lua
-- Simply modify component

local transform = self:getTransformComponent()
local pos = transform.boundingBox.position
pos.x = pos.x + 1
```
