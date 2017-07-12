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
