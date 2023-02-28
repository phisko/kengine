project(kengine)

system_link_private_libraries(kengine_glm)

find_package(unofficial-recast CONFIG REQUIRED)
system_link_public_libraries(recast)