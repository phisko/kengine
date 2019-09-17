set(name kengine_bullet)

set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_CPU_DEMOS OFF CACHE BOOL "" FORCE)
set(USE_GLUT OFF CACHE BOOL "" FORCE)
set(BUILD_ENET OFF CACHE BOOL "" FORCE)
set(BUKD_CLSOCKET OFF CACHE BOOL "" FORCE)
set(BUILD_EGL OFF CACHE BOOL "" FORCE)
SET(BUILD_OPENGL3_DEMOS OFF CACHE BOOL "" FORCE)
SET(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
SET(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
SET(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/bullet3)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PUBLIC kengine
	LinearMath
	Bullet3Common
	BulletInverseDynamics
	BulletCollision
	BulletDynamics
	BulletSoftBody
)
target_include_directories(${name} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/libs/bullet3/src
)
