set(INC_ROOT ${PROJECT_SOURCE_DIR}/include/Box2D)
set(SRC_ROOT ${PROJECT_SOURCE_DIR}/src)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(BOX2D_COLLISION_SRCS
	${SRC_ROOT}/Collision/BroadPhase.cpp
	${SRC_ROOT}/Collision/CollideCircle.cpp
	${SRC_ROOT}/Collision/CollideEdge.cpp
	${SRC_ROOT}/Collision/CollidePolygon.cpp
	${SRC_ROOT}/Collision/Collision.cpp
	${SRC_ROOT}/Collision/Distance.cpp
	${SRC_ROOT}/Collision/DynamicTree.cpp
	${SRC_ROOT}/Collision/TimeOfImpact.cpp
)
set(BOX2D_COLLISION_HDRS
	${INC_ROOT}/Collision/BroadPhase.hpp
	${INC_ROOT}/Collision/Collision.hpp
	${INC_ROOT}/Collision/Distance.hpp
	${INC_ROOT}/Collision/DynamicTree.hpp
	${INC_ROOT}/Collision/TimeOfImpact.hpp
)
set(BOX2D_SHAPES_SRCS
	${SRC_ROOT}/Collision/Shapes/CircleShape.cpp
	${SRC_ROOT}/Collision/Shapes/EdgeShape.cpp
	${SRC_ROOT}/Collision/Shapes/ChainShape.cpp
	${SRC_ROOT}/Collision/Shapes/PolygonShape.cpp
)
set(BOX2D_SHAPES_HDRS
	${INC_ROOT}/Collision/Shapes/CircleShape.hpp
	${INC_ROOT}/Collision/Shapes/EdgeShape.hpp
	${INC_ROOT}/Collision/Shapes/ChainShape.hpp
	${INC_ROOT}/Collision/Shapes/PolygonShape.hpp
	${INC_ROOT}/Collision/Shapes/Shape.hpp
)
set(BOX2D_COMMON_SRCS
	${SRC_ROOT}/Common/BlockAllocator.cpp
	${SRC_ROOT}/Common/Draw.cpp
	${SRC_ROOT}/Common/Math.cpp
	${SRC_ROOT}/Common/Settings.cpp
	${SRC_ROOT}/Common/StackAllocator.cpp
	${SRC_ROOT}/Common/Timer.cpp
)
set(BOX2D_COMMON_HDRS
	${INC_ROOT}/Common/BlockAllocator.hpp
	${INC_ROOT}/Common/Draw.hpp
	${INC_ROOT}/Common/GrowableStack.hpp
	${INC_ROOT}/Common/Math.hpp
	${INC_ROOT}/Common/Settings.hpp
	${INC_ROOT}/Common/StackAllocator.hpp
	${INC_ROOT}/Common/Timer.hpp
)
set(BOX2D_DYNAMICS_SRCS
	${SRC_ROOT}/Dynamics/Body.cpp
	${SRC_ROOT}/Dynamics/ContactManager.cpp
	${SRC_ROOT}/Dynamics/Fixture.cpp
	${SRC_ROOT}/Dynamics/Island.cpp
	${SRC_ROOT}/Dynamics/World.cpp
	${SRC_ROOT}/Dynamics/WorldCallbacks.cpp
)
set(BOX2D_DYNAMICS_HDRS
	${INC_ROOT}/Dynamics/Body.hpp
	${INC_ROOT}/Dynamics/ContactManager.hpp
	${INC_ROOT}/Dynamics/Fixture.hpp
	${INC_ROOT}/Dynamics/Island.hpp
	${INC_ROOT}/Dynamics/TimeStep.hpp
	${INC_ROOT}/Dynamics/World.hpp
	${INC_ROOT}/Dynamics/WorldCallbacks.hpp
)
set(BOX2D_CONTACTS_SRCS
	${SRC_ROOT}/Dynamics/Contacts/CircleContact.cpp
	${SRC_ROOT}/Dynamics/Contacts/Contact.cpp
	${SRC_ROOT}/Dynamics/Contacts/ContactSolver.cpp
	${SRC_ROOT}/Dynamics/Contacts/PolygonAndCircleContact.cpp
	${SRC_ROOT}/Dynamics/Contacts/EdgeAndCircleContact.cpp
	${SRC_ROOT}/Dynamics/Contacts/EdgeAndPolygonContact.cpp
	${SRC_ROOT}/Dynamics/Contacts/ChainAndCircleContact.cpp
	${SRC_ROOT}/Dynamics/Contacts/ChainAndPolygonContact.cpp
	${SRC_ROOT}/Dynamics/Contacts/PolygonContact.cpp
)
set(BOX2D_CONTACTS_HDRS
	${INC_ROOT}/Dynamics/Contacts/CircleContact.hpp
	${INC_ROOT}/Dynamics/Contacts/Contact.hpp
	${INC_ROOT}/Dynamics/Contacts/ContactSolver.hpp
	${INC_ROOT}/Dynamics/Contacts/PolygonAndCircleContact.hpp
	${INC_ROOT}/Dynamics/Contacts/EdgeAndCircleContact.hpp
	${INC_ROOT}/Dynamics/Contacts/EdgeAndPolygonContact.hpp
	${INC_ROOT}/Dynamics/Contacts/ChainAndCircleContact.hpp
	${INC_ROOT}/Dynamics/Contacts/ChainAndPolygonContact.hpp
	${INC_ROOT}/Dynamics/Contacts/PolygonContact.hpp
)
set(BOX2D_JOINTS_SRCS
	${SRC_ROOT}/Dynamics/Joints/DistanceJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/FrictionJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/GearJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/Joint.cpp
	${SRC_ROOT}/Dynamics/Joints/MotorJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/MouseJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/PrismaticJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/PulleyJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/RevoluteJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/RopeJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/WeldJoint.cpp
	${SRC_ROOT}/Dynamics/Joints/WheelJoint.cpp
)
set(BOX2D_JOINTS_HDRS
	${INC_ROOT}/Dynamics/Joints/DistanceJoint.hpp
	${INC_ROOT}/Dynamics/Joints/FrictionJoint.hpp
	${INC_ROOT}/Dynamics/Joints/GearJoint.hpp
	${INC_ROOT}/Dynamics/Joints/Joint.hpp
	${INC_ROOT}/Dynamics/Joints/MotorJoint.hpp
	${INC_ROOT}/Dynamics/Joints/MouseJoint.hpp
	${INC_ROOT}/Dynamics/Joints/PrismaticJoint.hpp
	${INC_ROOT}/Dynamics/Joints/PulleyJoint.hpp
	${INC_ROOT}/Dynamics/Joints/RevoluteJoint.hpp
	${INC_ROOT}/Dynamics/Joints/RopeJoint.hpp
	${INC_ROOT}/Dynamics/Joints/WeldJoint.hpp
	${INC_ROOT}/Dynamics/Joints/WheelJoint.hpp
)
set(BOX2D_ROPE_SRCS
	${SRC_ROOT}/Rope/Rope.cpp
)
set(BOX2D_ROPE_HDRS
	${INC_ROOT}/Rope/Rope.hpp
)
set(BOX2D_GENERAL_HDRS
	${INC_ROOT}/Box2D.hpp
)

# Library to build
set(BOX2D_LIB ${PROJECT_NAME})

if(BOX2D_BUILD_SHARED)
	add_library(${BOX2D_LIB} SHARED
		${BOX2D_JOINTS_SRCS}
		${BOX2D_CONTACTS_SRCS}
		${BOX2D_DYNAMICS_SRCS}
		${BOX2D_COMMON_SRCS}
		${BOX2D_SHAPES_SRCS}
		${BOX2D_COLLISION_SRCS}
		${BOX2D_ROPE_SRCS}
	)
	set_target_properties(${BOX2D_LIB} PROPERTIES DEBUG_POSTFIX -d
		VERSION ${BOX2D_VERSION}
	)
	if(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
		set_target_properties(${BOX2D_LIB} PROPERTIES IMPORT_SUFFIX ".a"
			VERSION ${BOX2D_VERSION}
		)
	endif()
else()
	add_library(${BOX2D_LIB} STATIC
		${BOX2D_JOINTS_SRCS}
		${BOX2D_CONTACTS_SRCS}
		${BOX2D_DYNAMICS_SRCS}
		${BOX2D_COMMON_SRCS}
		${BOX2D_SHAPES_SRCS}
		${BOX2D_COLLISION_SRCS}
		${BOX2D_ROPE_SRCS}
	)
	set_target_properties(${BOX2D_LIB} PROPERTIES DEBUG_POSTFIX -s-d
		VERSION ${BOX2D_VERSION}
	)
	set_target_properties(${BOX2D_LIB} PROPERTIES RELEASE_POSTFIX -s
		VERSION ${BOX2D_VERSION}
	)
endif()

# These are used to create visual studio folders.
source_group(Collision FILES ${BOX2D_COLLISION_SRCS} ${BOX2D_COLLISION_HDRS})
source_group(Collision\\Shapes FILES ${BOX2D_SHAPES_SRCS} ${BOX2D_SHAPES_HDRS})
source_group(Common FILES ${BOX2D_COMMON_SRCS} ${BOX2D_COMMON_HDRS})
source_group(Dynamics FILES ${BOX2D_DYNAMICS_SRCS} ${BOX2D_DYNAMICS_HDRS})
source_group(Dynamics\\Contacts FILES ${BOX2D_CONTACTS_SRCS} ${BOX2D_CONTACTS_HDRS})
source_group(Dynamics\\Joints FILES ${BOX2D_JOINTS_SRCS} ${BOX2D_JOINTS_HDRS})
source_group(Include FILES ${BOX2D_GENERAL_HDRS})
source_group(Rope FILES ${BOX2D_ROPE_SRCS} ${BOX2D_ROPE_HDRS})

# Install headers
install(FILES ${BOX2D_GENERAL_HDRS}   DESTINATION include/Box2D)
install(FILES ${BOX2D_COLLISION_HDRS} DESTINATION include/Box2D/Collision)
install(FILES ${BOX2D_SHAPES_HDRS}    DESTINATION include/Box2D/Collision/Shapes)
install(FILES ${BOX2D_COMMON_HDRS}    DESTINATION include/Box2D/Common)
install(FILES ${BOX2D_DYNAMICS_HDRS}  DESTINATION include/Box2D/Dynamics)
install(FILES ${BOX2D_CONTACTS_HDRS}  DESTINATION include/Box2D/Dynamics/Contacts)
install(FILES ${BOX2D_JOINTS_HDRS}    DESTINATION include/Box2D/Dynamics/Joints)
install(FILES ${BOX2D_ROPE_HDRS}      DESTINATION include/Box2D/Rope)

# Install CMake module
install(FILES ${PROJECT_SOURCE_DIR}/cmake/FindBox2D.cmake DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake)

# Install library (use "bin" folder for DLL, "lib" for SO files)
if(WIN32)
	install(TARGETS ${BOX2D_LIB}
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
	)
else()
	install(TARGETS ${BOX2D_LIB}
			RUNTIME DESTINATION lib
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib
	)
endif()
