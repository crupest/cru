cmake_minimum_required(VERSION 3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

project(cru)

if (APPLE)
	include_directories(BEFORE SYSTEM "/usr/local/include")
endif()

add_subdirectory(lib/Catch2)
add_subdirectory(lib/double-conversion)

enable_testing()

if (MSVC)
	add_compile_options(/utf-8)
endif()

set(CRU_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

# We don't use meson at present. Enable it in future.
# include(scripts/cmake/meson-projects.cmake)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(demos)

set(CLANGD_FILE ${CMAKE_BINARY_DIR}/clangd)

if(EMSCRIPTEN)
	execute_process(COMMAND ${CMAKE_C_COMPILER} --cflags OUTPUT_VARIABLE CLANGD_FLAGS_TO_ADD)
	separate_arguments(CLANGD_FLAGS_TO_ADD UNIX_COMMAND "${CLANGD_FLAGS_TO_ADD}")
	list(JOIN CLANGD_FLAGS_TO_ADD ", " CLANGD_FLAGS_TO_ADD)
endif()

set(CLANGD_TEMPLATE ${PROJECT_SOURCE_DIR}/scripts/clangd.in)
configure_file(${CLANGD_TEMPLATE} ${CLANGD_FILE})

