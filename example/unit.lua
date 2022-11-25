-- Simply modify component

local transform = self:get_transform()
local pos = transform.bounding_box.position
pos.x = pos.x + 1
