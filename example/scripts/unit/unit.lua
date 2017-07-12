-- huhuhu, modifying components test

local pos = self:getTransformComponent().boundingBox.topLeft
if (pos.x >= 10) then
    pos.x = 0
    print("[scripts/unit/unit.lua] Moving ", self:getName(), " back to x = 0")
end
