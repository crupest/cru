cmake_minimum_required(VERSION 3.14)

if(NOT CMAKE_TOOLCHAIN_FILE)
	message("CMAKE_TOOLCHAIN_FILE not set. Begin to find vcpkg toolchain file.")
    if(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
		message("VCPKG_INSTALLATION_ROOT environment variable is set. Use that vcpkg.")
		set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake
		CACHE STRING "Vcpkg toolchain file")
	else()
		message("VCPKG_INSTALLATION_ROOT environment variable is NOT set. Use local vcpkg.")
		set(CMAKE_TOOLCHAIN_FILE vcpkg/scripts/buildsystems/vcpkg.cmake
		CACHE STRING "Vcpkg toolchain file")
	endif()
endif()

set(CMAKE_CXX_STANDARD 20)

project(cru)

enable_testing()

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if (MSVC)
	string(REGEX REPLACE "/W[0-4]\\s*" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	add_compile_options(/utf-8 /W4 /WX)
endif()

set(CRU_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(demos)
