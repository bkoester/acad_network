cmake_minimum_required(VERSION 3.0.0)  # for CMAKE_CXX_COMPILER_ID
project (thesis)

# set some locations
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(GMOCK_ROOT "$ENV{HOME}/links/third_party/gmock-1.7.0")

# Unit testing with Google Test
option(unit_tests "Build unit tests" OFF)

# Option for building swig hooks
option(swig_hooks "Build swig hooks into C++ for Python.")

if (swig_hooks)
	message(STATUS "Swig hook targets available.")
	find_package(SWIG REQUIRED)
	include(${SWIG_USE_FILE})

	find_package(PythonLibs)
endif()

#these BOOST directories need to be checked with the right paths.
#I'm using my own install, couldn't find the path on flux.
site_name(SITE_NAME)
if (${SITE_NAME} MATCHES ".*flux.*")
	set(BOOST_ROOT "/home/bkoester/src/boost/boost_1_57_0/")
	set(BOOST_INCLUDEDIR "/home/bkoester/src/boost/boost_1_57_0/boost")
	set(BOOST_LIBRARYDIR "/home/bkoester/src/boost/boost_1_57_0/libs")
	set(Boost_NO_SYSTEM_PATHS ON)
	set(GMOCK_ROOT "/afs/umich.edu/user/k/a/karepker/links/third_party/gmock-1.7.0/")
	message(STATUS "Using flux configuration.")
endif()

find_package(Boost 1.57.0 REQUIRED serialization program_options)
message(STATUS "Boost including from ${Boost_INCLUDE_DIRS}")

find_package(Threads)

# set compilation variables
if (CMAKE_COMPILER_IS_GNUCC)
	if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.8)
		message(FATAL_ERROR
            "Cannot use gcc version ${CMAKE_CXX_COMPILER_VERSION} < 4.8!")
	elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.9)
		set(CXXSTD "-std=c++11")
	else ()
		set(CXXSTD "-std=c++14")
	endif()

	# -Wno-missing-field-initializers for swig
	set(COMMON_FLAGS "-Wno-missing-field-initializers \
		-Wall -Wextra -Werror -pedantic-errors ${CXXSTD}")
    set(CMAKE_CXX_FLAGS ${COMMON_FLAGS})
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0")
	set(SWIG_COMPILE_FLAGS "-fPIC -Wno-missing-field-initializers")

elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
		# AppleClang versions mapped to vanilla clang versions using:
		# https://gist.github.com/yamaya/2924292
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 5.1)
			message(FATAL_ERROR "Cannot use AppleClang version \
				${CMAKE_CXX_COMPILER_VERSION} < 5.1!")
		elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 6.0)
			set(CXXSTD "-std=c++11")
		else ()
			set(CXXSTD "-std=c++14")
		endif()
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		if (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.3)
			message(FATAL_ERROR
				"Cannot use clang version ${CMAKE_CXX_COMPILER_VERSION} < 3.3!")
		elseif (${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.4)
			set(CXXSTD "-std=c++11")
		else ()
			set(CXXSTD "-std=c++14")
		endif()
	else ()
		message(FATAL_ERROR "Unknown clang dialect in use!")
	endif()

	# -Wno-missing-field-initializers for swig
	set(COMMON_FLAGS "-Wall -Wextra -Werror -Wno-missing-field-initializers \
	-pedantic-errors ${CXXSTD}")
    set(CMAKE_CXX_FLAGS ${COMMON_FLAGS})
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0")
	set(SWIG_COMPILE_FLAGS "-fPIC")
else ()
	message(FATAL_ERROR "Unsupported compiler! To add support, add a section \
	to CMakeLists.txt specifying flags for this compiler.")
endif()


# add directory for project source files
add_subdirectory(${SOURCE_DIR})

message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")
