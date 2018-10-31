# Kengine

The Koala engine is a type-safe and self-documenting implementation of an Entity-Component-System (ECS), with a focus on runtime extensibility and compile-time type safety and clarity.

![koala](koala.png)

## Installation

The project uses git submodules extensively, and must therefore be cloned recursively.

```
git clone --recursive git@github.com:phiste/kengine
```

Alternatively, if using this resository as a submodule of your own project, make sure to init, sync and update your submodules in order to make sure the "putils" folder isn't empty.

The engine requires a **C++17** compiler.

### Classes

* [Component](Component.md): contains information about a certain property of an entity (for instance, a `TransformComponent` might hold an entity's position and size)
* [GameObject](GameObject.md): represents an in-game entity. Is simply a container of `Components`
* [System](System.md): holds game logic. A `PhysicsSystem` might control the movement of `GameObjects`, for instance.
* [EntityManager](EntityManager.md): manages `GameObjects`, `Components` and `Systems`
* [EntityFactory](EntityFactory.md): used to create `GameObjects` typed at run-time (by replacing template parameters by strings)

### Samples

These are pre-built, extensible and pluggable elements that can be used in any project, or to bootstrap a project.

##### Components

* [LuaComponent](common/components/LuaComponent.md): defines the lua scripts to be run by the `LuaSystem` for a `GameObject`
* [PyComponent](common/components/PyComponent.md): defines the Python scripts to be run by the `PySystem` for a `GameObject`
* [InputComponent](common/components/InputComponent.md): lets `GameObjects` receive keyboard and mouse events
* [ImGuiComponent](common/components/ImGuiComponent.md): lets `GameObjects` render debug elements using [ImGui](https://github.com/ocornut/imgui/)
* [GraphicsComponent](common/components/GraphicsComponent.md): provides graphical information about a `GameObject`, such as its appearance, used by the `SfSystem`
* [TransformComponent](common/components/TransformComponent.md): defines a `GameObject`'s position and size
* [PhysicsComponent](common/components/PhysicsComponent.md): defines a `GameObject`'s movement

##### Systems

* [LogSystem](common/systems/LogSystem.md): logs messages
* [LuaSystem](common/systems/LuaSystem.md): executes lua scripts, either global or attached to an entity
* [PySystem](common/systems/PySystem.md): executes Python scripts, either global or attached to an entity
* [PhysicsSystem](common/systems/PhysicsSystem.md): moves entities in a framerate-independent way
* [CollisionSystem](common/systems/CollisionSystem.md): transfers collision notifications to `GameObjects`
* [Box2DSystem](common/systems/box2d/Box2DSystem.md): performs the same duties as the `PhysicsSystem`, but using the **Box2D** library
* [SfSystem](common/systems/sfml/SfSystem.md): displays entities in an SFML render window

##### DataPackets

* [Log](common/packets/Log.hpp): received by the `LogSystem`, used to log a message
* [Collision](common/packets/Collision.hpp): sent by the `PhysicsSystem`, indicates a collision between two `GameObjects`
* [RegisterAppearance](common/packets/RegisterAppearance.hpp): received by the `SfSystem`, maps an abstract appearance to a concrete texture file.

These are datapackets sent from one `System` to another to communicate.

### Usage

For a quick start, look at [this](https://github.com/phiste/flappy_koala) example project, or any of the examples below.

### Example

Here is a list of simple, half-a-day implementation of games:

* [Flappy bird clone](https://github.com/phiste/flappy_koala)
* [Tunnel game, dodging cubes](https://github.com/phiste/koala_tunnel)
* [Tower defense "game", not much at stake though](https://github.com/phiste/koala_defense)
* [Shoot'em up game](https://github.com/phiste/shmup)



A more advanced, work-in-progress POC game using the engine with 3D graphics is available [here](https://github.com/phiste/hackemup).

Below is a commented main function that creates an entity and attaches some components to it, as well as some lua scripts (one of which is attached to the entity, while the other is run as a "system"). These should let you get an idea of what is possible using the kengine's support for reflection and runtime extensibility, as well as the compile-time clarity and type-safety that were the two motivations behind the project.

##### main.cpp

```cpp
#include <iostream>

#include "go_to_bin_dir.hpp"

#include "EntityManager.hpp"
#include "GameObject.hpp"

#include "common/systems/LuaSystem.hpp"
#include "common/systems/LogSystem.hpp"
#include "common/components/GraphicsComponent.hpp"
#include "common/components/TransformComponent.hpp"

int main(int, char **av)
{
    // Go to the executable's directory to be next to resources and scripts
    putils::goToBinDir(av[0]);

    // Create an EntityManager
    kengine::EntityManager em(std::make_unique<kengine::ExtensibleFactory>());

    // Load the specified systems, and any plugin placed in the executable's directory
    //      If you specify 'KENGINE_SFML' as TRUE in your CMakeLists.txt, this will load the SfSystem
    em.loadSystems<kengine::LuaSystem, kengine::LogSystem>(".");

    // To add a new system, simply add a DLL with a
    //      `ISystem *getSystem(kengine::EntityManager &em)`
    // function to the executable directory


    // Get the factory and register any desired types
    auto &factory = em.getFactory<kengine::ExtensibleFactory>();
    factory.registerTypes<kengine::GameObject>();

    // Create a GameObject and attach Components to it
    auto &player = em.createEntity<kengine::GameObject>("player");
    player.attachComponent<kengine::TransformComponent3f>();
    player.attachComponent<kengine::GraphicsComponent>();

    // Attach a lua script to a GameObject
    auto &luaComp = player.attachComponent<kengine::LuaComponent>();
    luaComp.attachScript("scripts/unit/unit.lua");

    // Register types to be used in lua and add a directory of scripts to be executed
    try
    {
        auto &lua = em.getSystem<kengine::LuaSystem>();
        lua.addScriptDirectory("scripts"); // The LuaSystem automatically opens the "scripts" directory, this is just an example
        lua.registerTypes<
                kengine::GraphicsComponent,
                kengine::TransformComponent3f, putils::Point<float, 3>, putils::Rect<float, 3>,
                kengine::LuaComponent,
                kengine::packets::Log
        >();
    }
    catch (const std::out_of_range &) {} // If the LuaSystem wasn't found, ignore

    // Start game
    while (em.running)
        em.execute();

    return (EXIT_SUCCESS);
}
```

##### scripts/test.lua

```lua
-- send a datapacket from Lua
local log = Log.new()
log.msg = "Log from lua"
sendLog(log)

-- create an entity from Lua
local new = createEntity("GameObject", "bob",
    function (go)
        print("Created " .. go:getName())
    end
)

-- get an entity
local otherRef = getEntity("bob")

-- attach a component
local meta = new:attachGraphicsComponent()
meta.appearance = "human"

local transform = new:attachTransformComponent()
transform.boundingBox.topLeft.x = 42

-- serialize
print(new)

-- iterate over all entities
for i, e in ipairs(getGameObjects()) do
    print(e)
end

-- iterate over specific entities
for i, e in ipairs(getGameObjectsWithGraphicsComponent()) do
    local comp = e:getTransformComponent()
    print(comp)
end

-- remove an Entity
removeEntity(new:getName())
```

##### scripts/unit/unit.lua

```lua
-- huhuhu, modifying components test

local pos = self:getTransformComponent().boundingBox.topLeft
if (pos.x >= 10) then
    pos.x = 0
    print("[scripts/unit/unit.lua] Moving ", self:getName(), " back to x = 0")
end
```
