project(kengine)

find_package(glm CONFIG REQUIRED)
kengine_library_link_public_libraries(glm::glm)