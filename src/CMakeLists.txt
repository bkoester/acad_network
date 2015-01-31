set(SRCS
	course.cpp
	course_network.cpp
	course_tab.cpp
	graph_builder.cpp
	student.cpp
	tab_reader.cpp)

set(MAIN_SRC main.cpp)

set(UNITTEST_SRCS
	course_test.cpp
	course_network_test.cpp
	course_tab_test.cpp
	graph_builder_test.cpp
	network_structure_test.cpp
	student_test.cpp
	student_network_test.cpp
	tab_reader_test.cpp
	utility_test.cpp)

set(DEBUG_OBJECT_LIBRARY "debug_objects")
set(DEBUG_BINARY "thesisd")
set(RELEASE_BINARY "thesis")
set(UNITTEST_BINARY "unittest")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${Boost_INCLUDE_DIRS})

add_library(${DEBUG_OBJECT_LIBRARY} OBJECT ${SRCS})
add_executable(${DEBUG_BINARY}
	${MAIN_SRC}
	$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)
add_executable(${RELEASE_BINARY} ${MAIN_SRC} ${SRCS})

set_property(SOURCE ${SRCS} PROPERTY COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(${DEBUG_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
set_target_properties(${RELEASE_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_RELEASE}")

target_link_libraries(${DEBUG_BINARY}
	${Boost_LIBRARIES})

target_link_libraries(${RELEASE_BINARY}
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

	add_executable(${UNITTEST_BINARY}
		${UNITTEST_SRCS}
		$<TARGET_OBJECTS:${DEBUG_OBJECT_LIBRARY}>)

	set_target_properties(${UNITTEST_BINARY} PROPERTIES COMPILE_FLAGS "${CXXFLAGS_DEBUG}")
	target_link_libraries(${UNITTEST_BINARY}
		${Boost_LIBRARIES}
		gmock_main
		pthread)

endif()
