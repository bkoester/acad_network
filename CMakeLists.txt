cmake_minimum_required(VERSION 2.8.8)  # for object libraries
project (thesis)

# set some locations
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(GMOCK_ROOT "$ENV{HOME}/links/third_party/gmock-1.7.0")

find_package(Boost REQUIRED serialization)

# set compilation variables
if (CMAKE_COMPILER_IS_GNUCC)
	if (GCC_VERSION VERSION_LESS 4.9)
		set(CXX14FLAG "-std=c++1y")
	elseif (GCC_VERSION VERSION_GREATER 4.8)
		set(CXX14FLAG "-std=c++14")
	else()
		message(FATAL_ERROR "Cannot use gcc lower than 4.8")
	endif()
	
	set(CXXFLAGS "-Wall -Werror -pedantic-errors ${CXX14FLAG}")
	set(CXXFLAGS_RELEASE "${CXXFLAGS} -O3 -DNDEBUG")
	set(CXXFLAGS_DEBUG "${CXXFLAGS} -g -O0")
endif()


# add directory for project source files
add_subdirectory(${SOURCE_DIR})

# Unit testing with Google Test
option(unit_tests "Build unit tests" OFF)
