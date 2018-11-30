# Kengine

The Koala engine is a type-safe and self-documenting implementation of an Entity-Component-System (ECS), with a focus on runtime extensibility and compile-time type safety and clarity.

This new version features a greatly optimized memory architecture, better cache-efficiency, and a more advanced API.

It has however (temporarily) lost a couple of features:
* Entities no longer have names. If you really need to associate strings to your Entities you can create a `NameComponent` or something similar
* Entities can no longer be serialized to `ostream`. I am currently working to re-implement this feature.
* Because of the previous point, it is no longer possible to `save` and `load` the game state. I am currently working to re-implement this feature, in a manner that will be better optimized than the original
(binary saving instead of JSON serialization)

![koala](koala.png)

## Installation

The project uses git submodules extensively, and must therefore be cloned recursively.

```
git clone --recursive git@github.com:phiste/kengine
```

Alternatively, if using this resository as a submodule of your own project, make sure to init, sync and update your submodules in order to make sure the "putils" folder isn't empty.

The engine requires a **C++17** compiler.

### Classes

* [Entity](Entity.md): represents an in-game entity. Is simply a container of `Components`
* [System](System.md): holds game logic. A `PhysicsSystem` might control the movement of `Entities`, for instance.
* [EntityManager](EntityManager.md): manages `Entities`, `Components` and `Systems`

Note that there is no `Component` class. Any type can be used as a `Component`, and dynamically attached/detached to `Entities`.

### Samples

These are pre-built, extensible and pluggable elements that can be used in any project, or to bootstrap a project.

##### Components

General purpose gamedev:
* [TransformComponent](common/components/TransformComponent.md): defines a `Entity`'s position and size
* [PhysicsComponent](common/components/PhysicsComponent.md): defines a `Entity`'s movement

Behaviors:
* [BehaviorComponent](common/components/BehaviorComponent.md): defines a function to be called each frame for a `Entity`
* [LuaComponent](common/components/LuaComponent.md): defines the lua scripts to be run by the `LuaSystem` for a `Entity`
* [PyComponent](common/components/PyComponent.md): defines the Python scripts to be run by the `PySystem` for a `Entity`
* [CollisionComponent](common/components/CollisionComponent.md): defines a function to be called when a `Entity` collides with another

Debug tools:
* [AdjustableComponent](common/components/AdjustableComponent.md): lets users modify variables through a GUI (such as the [ImGuiAdjustableManager](common/gameobjects/ImGuiAdjustableManager.md))
* [ImGuiComponent](common/components/ImGuiComponent.md): lets `Entities` render debug elements using [ImGui](https://github.com/ocornut/imgui/)
* [DebugGraphicsComponent](common/components/DebugGraphicsComponent.hpp): lets a `Entity` be used to draw debug information (such as lines, rectangles or spheres)

Graphics and gamedev:
* [GraphicsComponent](common/components/GraphicsComponent.md): provides graphical information about a `Entity`, such as its appearance, used by the `SfSystem`
* [CameraComponent](common/components/CameraComponent.hpp): lets `Entities` be used as in-game cameras, to define a frustrum and position. Follows the same conventions as `TransformComponent`
* [InputComponent](common/components/InputComponent.md): lets `Entities` receive keyboard and mouse events
* [GUIComponent](common/components/GUIComponent.md): lets `Entities` be used as GUI elements such as buttons, lists...)

##### Systems

* [BehaviorSystem](common/systems/BehaviorSystem.md): executes behaviors attached to `Entities`
* [CollisionSystem](common/systems/CollisionSystem.md): transfers collision notifications to `Entities`
* [LogSystem](common/systems/LogSystem.md): logs messages
* [LuaSystem](common/systems/LuaSystem.md): executes lua scripts attached to an entity
* [PySystem](common/systems/PySystem.md): executes Python scripts attached to an entity
* [PhysicsSystem](common/systems/PhysicsSystem.md): moves entities in a framerate-independent way
* [SfSystem](common/systems/sfml/SfSystem.md): displays entities in an SFML render window

##### DataPackets

* [Log](common/packets/Log.hpp): received by the `LogSystem`, used to log a message
* [Collision](common/packets/Collision.hpp): sent by the `PhysicsSystem`, indicates a collision between two `Entities`
* [RegisterAppearance](common/packets/RegisterAppearance.hpp): received by the `SfSystem`, maps an abstract appearance to a concrete texture file.

These are datapackets sent from one `System` to another to communicate.

### Example

Here is a list of simple, half-a-day implementation of games:

* [Bots painting a canvas](https://github.com/phisko/painter)

Old API, deprecated:
* [Flappy bird clone](https://github.com/phisko/flappy_koala)
* [Tunnel game, dodging cubes](https://github.com/phisko/koala_tunnel)
* [Tower defense "game", not much at stake though](https://github.com/phisko/koala_defense)
* [Shoot'em up game](https://github.com/phisko/shmup)

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
local pos = transform.boundingBox.topLeft
pos.x = pos.x + 1
```
