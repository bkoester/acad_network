set(STUDENTS_COURSES_SRCS
	course.cpp
	course_container.cpp
	course_network.cpp
	mem_usage.cpp
	student.cpp
	student_container.cpp
	utility.cpp
	)

set(BUILD_SRCS
	graph_builder.cpp
	weighting_function.cpp
	)

set(BUILD_MAIN_SRC build_main.cpp)

set(LOAD_SRCS
	)

SET(LOAD_MAIN_SRCS
	network_processing/degree_summation.cpp
	network_processing/individual_distances.cpp
	network_processing/individual_network.cpp
	network_processing/reduce_network.cpp
	)

set(SERIALIZE_MAIN_SRC serialize_main.cpp)

set(STUDENTS_COURSES_UNITTEST_SRCS
	course_test.cpp
	course_container_test.cpp
	course_network_test.cpp
	network_test.cpp
	network_structure_test.cpp
	student_test.cpp
	student_network_test.cpp
	student_container_test.cpp
	utility_test.cpp
	)

set(BUILD_UNITTEST_SRCS
	graph_builder_test.cpp
	weighting_function_test.cpp
	)

set(LOAD_UNITTEST_SRCS
	reduce_network_test.cpp
	)

set(SERIALIZE_UNITTEST_SRCS
	)

set(SWIG_SRCS
    course.i
    course_container.i
    student.i
    student_container.i
    )

set(STUDENTS_COURSES_LIBRARY "students_courses")
set(BUILD_BINARY "build")
set(SERIALIZE_BINARY "serialize")
set(STUDENTS_COURSES_UNITTEST_BINARY "students_courses_unittest")
set(STUDENTS_COURSES_UNITTEST_LIBRARY "students_courses_unittest_objects")
set(BUILD_UNITTEST_BINARY "build_unittest")
set(LOAD_UNITTEST_BINARY "load_unittest")
set(SERIALIZE_UNITTEST_BINARY "serialize_unittest")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

if (swig_hooks)
	include_directories(SYSTEM ${PYTHON_INCLUDE_PATH})

	set(CMAKE_SWIG_FLAGS "")

	set_source_files_properties(${SWIG_SRCS} PROPERTIES
		CPLUSPLUS ON)

	set(SWIG_MODULE_student_container_EXTRA_DEPS
		student.i
		student_container.hpp
		)

	set(SWIG_MODULE_course_container_EXTRA_DEPS
		course.i
		course_container.hpp
		)

	swig_add_module(student python student.i)
	swig_add_module(student_container python student_container.i)
	swig_add_module(course python course.i)
	swig_add_module(course_container python course_container.i)


	set(STUDENT_COURSES_SWIG_LINK_LIBRARIES
		${PYTHON_LIBRARIES}
		${STUDENTS_COURSES_LIBRARY}
		)

	swig_link_libraries(student ${STUDENT_COURSES_SWIG_LINK_LIBRARIES})
	swig_link_libraries(student_container ${STUDENT_COURSES_SWIG_LINK_LIBRARIES})
	swig_link_libraries(course ${STUDENT_COURSES_SWIG_LINK_LIBRARIES})
	swig_link_libraries(course_container ${STUDENT_COURSES_SWIG_LINK_LIBRARIES})
endif()

# make this library shared as recommended for linking with swig modules
add_library(${STUDENTS_COURSES_LIBRARY} SHARED ${STUDENTS_COURSES_SRCS})
set_target_properties(${STUDENTS_COURSES_LIBRARY} PROPERTIES
    COMPILE_FLAGS ${SWIG_COMPILE_FLAGS}
    )

add_executable(${BUILD_BINARY}
	${BUILD_MAIN_SRC}
	${BUILD_SRCS})

add_executable(${SERIALIZE_BINARY}
	${SERIALIZE_MAIN_SRC}
	${SERIALIZE_SRCS})

set(BINARY_LINK_LIBRARIES
    ${Boost_LIBRARIES}
    ${STUDENTS_COURSES_LIBRARY}
	${CMAKE_THREAD_LIBS_INIT}
    )

target_link_libraries(${STUDENTS_COURSES_LIBRARY}
    ${Boost_LIBRARIES}
	)
target_link_libraries(${BUILD_BINARY} ${BINARY_LINK_LIBRARIES})
target_link_libraries(${SERIALIZE_BINARY} ${BINARY_LINK_LIBRARIES})

# make a separate binary for every file in the network_processing directory
foreach (load_main_src ${LOAD_MAIN_SRCS})
	get_filename_component(load_binary_name ${load_main_src} NAME_WE)
	add_executable(${load_binary_name}
		${load_main_src}
		${LOAD_SRCS}
		)
	target_link_libraries(${load_binary_name} ${BINARY_LINK_LIBRARIES})
endforeach(load_main_src)

# if we choose to build unit tests, add rules for building unittest executable
if (unit_tests)
    message(STATUS "Unit test targets available.")
	if (NOT EXISTS "${GMOCK_ROOT}")
		message("'${GMOCK_ROOT}' does not exist, please set the GMOCK_ROOT
		variable to the appropriate directory")
		return()
	endif()

	# add binaries of google test and google mock to their roots
	add_subdirectory("${GMOCK_ROOT}" "${GMOCK_ROOT}")

	include_directories(
		${GMOCK_ROOT}/gtest
		${GMOCK_ROOT}/gtest/include
		${GMOCK_ROOT}
		${GMOCK_ROOT}/include)

    add_library(${STUDENTS_COURSES_UNITTEST_LIBRARY} OBJECT
        ${STUDENTS_COURSES_UNITTEST_SRCS})

    add_executable(${STUDENTS_COURSES_UNITTEST_BINARY}
        ${STUDENTS_COURSES_SRCS}
        ${STUDENTS_COURSES_UNITTEST_SRCS}
        )

	add_executable(${BUILD_UNITTEST_BINARY}
		${BUILD_SRCS}
		${BUILD_UNITTEST_SRCS}
		$<TARGET_OBJECTS:${STUDENTS_COURSES_UNITTEST_LIBRARY}>
        )

	add_executable(${LOAD_UNITTEST_BINARY}
		${LOAD_SRCS}
		${LOAD_UNITTEST_SRCS}
		$<TARGET_OBJECTS:${STUDENTS_COURSES_UNITTEST_LIBRARY}>
        )

	add_executable(${SERIALIZE_UNITTEST_BINARY}
		${SERIALIZE_SRCS}
		${SERIALIZE_UNITTEST_SRCS}
		$<TARGET_OBJECTS:${STUDENTS_COURSES_UNITTEST_LIBRARY}>
        )

    # special case: we don't need to link STUDENTS_COURSES_LIBRARY because we
    # are testing it
    target_link_libraries(${STUDENTS_COURSES_UNITTEST_BINARY}
        ${Boost_LIBRARIES}
        gmock_main
        )

    set(UNITTEST_BINARY_LINK_LIBRARIES
        ${Boost_LIBRARIES}
        gmock_main
        pthread
        ${STUDENTS_COURSES_LIBRARY})

    target_link_libraries(${BUILD_UNITTEST_BINARY}
        ${UNITTEST_BINARY_LINK_LIBRARIES})

	target_link_libraries(${LOAD_UNITTEST_BINARY}
        ${UNITTEST_BINARY_LINK_LIBRARIES})

	target_link_libraries(${SERIALIZE_UNITTEST_BINARY}
        ${UNITTEST_BINARY_LINK_LIBRARIES})

endif()
