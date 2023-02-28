project(kengine)

system_link_private_libraries(
		kengine_glm
		kengine_skeleton
)

find_package(Bullet CONFIG REQUIRED)
system_link_private_libraries(${BULLET_LIBRARIES})