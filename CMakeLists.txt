cmake_minimum_required(VERSION 3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CRU_DEFAULT_BUILD_SHARED_LIBS ON)
if (EMSCRIPTEN)
	# WebAssembly has no idea about static/dynamic libraries.
	set(CRU_DEFAULT_BUILD_SHARED_LIBS OFF)
endif()

option(BUILD_SHARED_LIBS "Try to build shared libraries as default" CRU_DEFAULT_BUILD_SHARED_LIBS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

project(cru)

if (APPLE)
	include_directories(BEFORE SYSTEM "/usr/local/include")
endif()

add_subdirectory(lib/Catch2)
add_subdirectory(lib/double-conversion)
add_subdirectory(lib/GSL)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

enable_testing()

if (MSVC)
	add_compile_options(/utf-8)
endif()

set(CRU_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(CRU_ASSETS_DIR ${PROJECT_SOURCE_DIR}/assets)

function(target_add_resources target res_dir)
	message("Add resources for target ${target} with files ${res_dir}.")

	file(GLOB_RECURSE RES_SOURCES "${CRU_ASSETS_DIR}/${res_dir}/*")
	target_sources(${target} PUBLIC ${RES_SOURCES})
	foreach (RES_FILE ${RES_SOURCES})
		file(RELATIVE_PATH RES_PATH ${CRU_ASSETS_DIR} ${RES_FILE})
		cmake_path(GET RES_PATH PARENT_PATH RES_PATH)
		set_property(SOURCE ${RES_FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${RES_PATH}")
	endforeach(RES_FILE)
endfunction()

include(scripts/cmake/meson-projects.cmake)

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(demos)

set(CLANGD_FILE ${CMAKE_BINARY_DIR}/clangd)
if(EMSCRIPTEN)
	execute_process(COMMAND ${CMAKE_C_COMPILER} --cflags OUTPUT_VARIABLE CLANGD_FLAGS_TO_ADD)
	separate_arguments(CLANGD_FLAGS_TO_ADD UNIX_COMMAND "${CLANGD_FLAGS_TO_ADD}")
	list(JOIN CLANGD_FLAGS_TO_ADD ", " CLANGD_FLAGS_TO_ADD)
	set(CLANGD_TEMPLATE ${PROJECT_SOURCE_DIR}/scripts/clangd.in)
	configure_file(${CLANGD_TEMPLATE} ${CLANGD_FILE})
else()
	file(WRITE ${CLANGD_FILE} "")
endif()
