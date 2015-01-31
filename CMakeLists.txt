cmake_minimum_required(VERSION 2.8.10)  # for compiler version
project (thesis)

# set some locations
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(GMOCK_ROOT "$ENV{HOME}/links/third_party/gmock-1.7.0")

site_name(SITE_NAME)
if (${SITE_NAME} MATCHES ".*flux.*")
	set(BOOST_ROOT "/home/software/rhel6/boost/1.57.0-gcc-4.8.0")
	set(BOOST_INCLUDEDIR "/home/software/rhel6/boost/1.57.0-gcc-4.8.0/include")
	set(BOOST_LIBRARYDIR "/home/software/rhel6/boost/1.57.0-gcc-4.8.0/lib")
	set(Boost_NO_SYSTEM_PATHS ON)
	message(STATUS "Using flux configuration.")
endif()

find_package(Boost 1.57.0 REQUIRED serialization program_options)
message(STATUS "Boost including from ${Boost_INCLUDE_DIRS}")

# set compilation variables
if (CMAKE_COMPILER_IS_GNUCC)
	if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.8)
		message(FATAL_ERROR "Cannot use gcc version ${CMAKE_CXX_COMPILER_VERSION} < 4.8!")
	elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.9)
		set(CXXSTD "-std=c++11")
	else ()
		set(CXXSTD "-std=c++14")
	endif()
	
	set(CXXFLAGS "-Wall -Werror -pedantic-errors ${CXXSTD}")
	set(CXXFLAGS_RELEASE "${CXXFLAGS} -O3 -DNDEBUG")
	set(CXXFLAGS_DEBUG "${CXXFLAGS} -g -O0")
endif()


# add directory for project source files
add_subdirectory(${SOURCE_DIR})

# Unit testing with Google Test
option(unit_tests "Build unit tests" OFF)
