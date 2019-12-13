cmake_minimum_required(VERSION 3.14)

project(CruUI)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
	string(REGEX REPLACE "/W[0-4]\\s*" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	add_compile_options(/utf-8 /W4 /WX)
endif()

set(CRU_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
