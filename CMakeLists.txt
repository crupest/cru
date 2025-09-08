cmake_minimum_required(VERSION 3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

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

function(cru_process_dir dir)
	get_property(targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
	foreach(target ${targets})
		if(WIN32)
			get_property(target_type TARGET "${target}" PROPERTY TYPE)
			if("${target_type}" STREQUAL "SHARED_LIBRARY")
				target_compile_definitions("${target}" PRIVATE CRU_IS_DLL=1)
			endif()
		endif()
	endforeach()
	get_property(subdirectories DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
	foreach(subdir ${subdirectories})
		cru_process_dir("${subdir}")
	endforeach()
endfunction()

cru_process_dir(".")
