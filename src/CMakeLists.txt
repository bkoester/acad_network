set(SRCS
	course.cpp
	course_network.cpp
	mem_usage.cpp
	student.cpp
	tab_reader.cpp
	utility.cpp
	)

set(BUILD_MAIN_SRCS 
	build_main.cpp
	graph_builder.cpp
	)

set(LOAD_MAIN_SRCS 
	load_main.cpp
	student_segmentation.cpp
	)

set(SEGMENT_SRCS 
	segment_main.cpp
	)

set(UNITTEST_SRCS
	course_test.cpp
	course_network_test.cpp
	network_test.cpp
	network_structure_test.cpp
	student_test.cpp
	student_network_test.cpp
	tab_reader_test.cpp
	utility_test.cpp
	)


set(BUILD_UNITTEST_SRCS
	graph_builder_test.cpp
	)

set(LOAD_UNITTEST_SRCS
	reduce_network_test.cpp
	student_segmentation_test.cpp
	)

set(SEGMENT_UNITTEST_SRCS
	)

set(DEBUG_OBJECT_LIBRARY "debug_objects")
set(UNITTEST_OBJECT_LIBRARY "unittest_objects")
set(RELEASE_OBJECT_LIBRARY "release_objects")
set(BUILD_DEBUG_BINARY "buildd")
set(BUILD_RELEASE_BINARY "build")
set(LOAD_DEBUG_BINARY "loadd")
set(LOAD_RELEASE_BINARY "load")
set(SEGMENT_DEBUG_BINARY "segmentd")
set(SEGMENT_RELEASE_BINARY "segment")
set(BUILD_UNITTEST_BINARY "build_unittest")
set(LOAD_UNITTEST_BINARY "load_unittest")
set(SEGMENT_UNITTEST_BINARY "segment_unittest")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${Boost_INCLUDE_DIRS})

add_library(${DEBUG_OBJECT_LIBRARY} OBJECT ${SRCS})
add_library(${RELEASE_OBJECT_LIBRARY} OBJECT ${SRCS})
add_executable(${BUILD_DEBUG_BINARY}
	${BUILD_MAIN_SRCS}
	$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)
add_executable(${BUILD_RELEASE_BINARY}
	${BUILD_MAIN_SRCS}
	$<TARGET_OBJECTS:${RELEASE_OBJECT_LIBRARY}>)
add_executable(${LOAD_DEBUG_BINARY}
	${LOAD_MAIN_SRCS}
	$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)
add_executable(${LOAD_RELEASE_BINARY}
	${LOAD_MAIN_SRCS}
	$<TARGET_OBJECTS:${RELEASE_OBJECT_LIBRARY}>)
add_executable(${SEGMENT_DEBUG_BINARY}
	${SEGMENT_SRCS}
	$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)
add_executable(${SEGMENT_RELEASE_BINARY}
	${SEGMENT_SRCS}
	$<TARGET_OBJECTS:${RELEASE_OBJECT_LIBRARY}>)


set_property(TARGET ${DEBUG_OBJECT_LIBRARY} PROPERTY COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_property(TARGET ${RELEASE_OBJECT_LIBRARY} PROPERTY COMPILE_FLAGS "${CXXFLAGS_RELEASE}")
set_target_properties(${LOAD_DEBUG_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(${LOAD_RELEASE_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_RELEASE}")
set_target_properties(${BUILD_DEBUG_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(${BUILD_RELEASE_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_RELEASE}")
set_target_properties(${SEGMENT_DEBUG_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(${SEGMENT_RELEASE_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_RELEASE}")


target_link_libraries(${BUILD_DEBUG_BINARY}
	${Boost_LIBRARIES})
target_link_libraries(${BUILD_RELEASE_BINARY}
	${Boost_LIBRARIES})
target_link_libraries(${LOAD_DEBUG_BINARY}
	${Boost_LIBRARIES})
target_link_libraries(${LOAD_RELEASE_BINARY}
	${Boost_LIBRARIES})
target_link_libraries(${SEGMENT_DEBUG_BINARY}
	${Boost_LIBRARIES})
target_link_libraries(${SEGMENT_RELEASE_BINARY}
	${Boost_LIBRARIES})


# if we choose to build unit tests, add rules for building unittest executable
if (unit_tests)
	if (NOT EXISTS "${GMOCK_ROOT}")
		message("'${GMOCK_ROOT}' does not exist, please set the GMOCK_ROOT
		variable to the appropriate directory")
		return()
	endif()

	# add binaries of google test and google mock to their their roots
	add_subdirectory("${GMOCK_ROOT}" "${GMOCK_ROOT}")

	include_directories(
		${GMOCK_ROOT}/gtest
		${GMOCK_ROOT}/gtest/include
		${GMOCK_ROOT}
		${GMOCK_ROOT}/include)

	add_library(${UNITTEST_OBJECT_LIBRARY} OBJECT ${UNITTEST_SRCS})

	add_executable(${BUILD_UNITTEST_BINARY}
		${UNITTEST_SRCS}
		$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)
	add_executable(${LOAD_UNITTEST_BINARY}
		${UNITTEST_SRCS}
		$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)
	add_executable(${SEGMENT_UNITTEST_BINARY}
		${UNITTEST_SRCS}
		$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)

	set_target_properties(${BUILD_UNITTEST_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
	set_target_properties(${LOAD_UNITTEST_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
	set_target_properties(${SEGMENT_UNITTEST_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")

	target_link_libraries(${BUILD_UNITTEST_BINARY}
		${Boost_LIBRARIES}
		gmock_main
		pthread)
	target_link_libraries(${LOAD_UNITTEST_BINARY}
		${Boost_LIBRARIES}
		gmock_main
		pthread)
	target_link_libraries(${SEGMENT_UNITTEST_BINARY}
		${Boost_LIBRARIES}
		gmock_main
		pthread)

endif()
