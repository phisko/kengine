-- Simply modify component

local transform = self:getTransformComponent()
local pos = transform.boundingBox.topLeft
pos.x = pos.x + 1

