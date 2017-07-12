# Kengine

The Koala engine is a type-safe and self-documenting implementation of an Entity-Component-System (ECS).

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
* [MetaComponent](common/components/MetaComponent.md): provides metadata about a `GameObject`, such as its appearance, used by the `SfSystem`
* [TransformComponent](common/components/TransformComponent.md): defines a `GameObject`'s position and size

##### Systems

* [LogSystem](common/systems/LogSystem.md): logs messages
* [LuaSystem](common/systems/LuaSystem.md): executes scripts, either global or attached to an entity
* [SfSystem](common/systems/sfml/SfSystem.md): displays entities in an SFML render window

##### DataPackets

* [Log](common/packets/Log.hpp): received by the `LogSystem`, used to log a message
* [RegisterAppearance](common/packets/RegisterAppearance.hpp): received by the `SfSystem`, maps an abstract appearance to a concrete texture file.

These are datapackets sent from one `System` to another to communicate.

### Example

##### main.cpp

```
#include <iostream>

#include "go_to_bin_dir.hpp"

#include "EntityManager.hpp"
#include "GameObject.hpp"

#include "common/systems/LuaSystem.hpp"
#include "common/systems/LogSystem.hpp"
#include "common/components/MetaComponent.hpp"
#include "common/components/TransformComponent.hpp"

int main(int, char **av)
{
    // Go to the executable's directory to be next to resources and scripts
    putils::goToBinDir(av[0]);

    // Create an EntityManager
    kengine::EntityManager em(std::make_unique<kengine::ExtensibleFactory>());

    // Load the specified systems, and any plugin placed in the executable's directory
    //      If you specify 'PUTILS_BUILD_PSE' as TRUE in your CMakeLists.txt, this will load the SfSystem

    // To add a new system, simply add a DLL with a
    //      `ISystem *getSystem(kengine::EntityManager &em)`
    // function to the "plugins" directory
    em.loadSystems<kengine::LuaSystem, kengine::LogSystem>(".");


    // Get the factory and register any desired types
    auto &factory = em.getFactory<kengine::ExtensibleFactory>();
    factory.addType("GameObject", [](auto name) { return std::make_unique<kengine::GameObject>(name); });

    // Create a GameObject and attach Components to it
    auto &player = em.createEntity<kengine::GameObject>("player");
    player.attachComponent<kengine::TransformComponent3d>();
    player.attachComponent<kengine::MetaComponent>();

    // Attach a lua script to a GameObject
    auto &luaComp = player.attachComponent<kengine::LuaComponent>();
    luaComp.attachScript("scripts/unit/unit.lua");

    // Add a directory of scripts to be executed
    try
    {
        auto &lua = em.getSystem<kengine::LuaSystem>();
        lua.addScriptDirectory("scripts");
        lua.registerTypes<
                kengine::MetaComponent,
                kengine::TransformComponent3d, putils::Point<double, 3>, putils::Rect<double, 3>,
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

```
-- send a datapacket from Lua
local log = Log.new()
log.msg = "Log from lua"
sendLog(log)

-- create an entity from Lua
local new = createEntity("GameObject", "bob")

-- get an entity
local otherRef = getEntity("bob")

-- attach a component
local meta = new:attachMetaComponent()
meta.appearance = "human"

local transform = new:attachTransformComponent()
transform.boundingBox.topLeft.x = 42

-- serialize
print(new)

-- iterate over all entities
for i, e in pairs(getGameObjects()) do
    print(e)
end

-- remove an Entity
removeEntity(new)
```

##### scripts/unit/unit.lua

```
-- huhuhu, modifying components test

local pos = self:getTransformComponent().boundingBox.topLeft
if (pos.x >= 10) then
    pos.x = 0
    print("[scripts/unit/unit.lua] Moving ", self:getName(), " back to x = 0")
end
```
