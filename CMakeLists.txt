cmake_minimum_required(VERSION 3.21)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

set(CRU_VCPKG_ROOT "${CMAKE_SOURCE_DIR}/vcpkg" CACHE PATH "Path to vcpkg root directory")

# We only use vcpkg on Windows. For other platforms, we use the ones from system package manager.
if(WIN32)
	set(CRU_VCPKG_TOOLCHAIN_FILE "${CRU_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
	if(EXISTS "${CRU_VCPKG_TOOLCHAIN_FILE}")
		set(CMAKE_TOOLCHAIN_FILE "${CRU_VCPKG_TOOLCHAIN_FILE}" CACHE STRING "Vcpkg toolchain file")
	else()
		message(WARNING "vcpkg toolchain file not found at ${CRU_VCPKG_TOOLCHAIN_FILE}. "
			"Please set CRU_VCPKG_ROOT to the correct path. "
			"Otherwise, necessary dependencies may not be found.")
	endif()
endif()

project(cru)

option(CRU_ASAN "Build this target with AddressSanitizer" OFF)

if(CRU_ASAN)
	if(MSVC)
		string(APPEND CMAKE_C_FLAGS " /fsanitize=address")
		string(APPEND CMAKE_CXX_FLAGS " /fsanitize=address")
	else()
		string(APPEND CMAKE_C_FLAGS " -fsanitize=address")
		string(APPEND CMAKE_CXX_FLAGS " -fsanitize=address")
		string(APPEND CMAKE_EXE_LINKER_FLAGS " -fsanitize=address")
		string(APPEND CMAKE_SHARED_LINKER_FLAGS " -fsanitize=address")
		string(APPEND CMAKE_MODULE_LINKER_FLAGS " -fsanitize=address")
	endif()
endif()

if (APPLE)
	include_directories(BEFORE SYSTEM "/usr/local/include")
endif()

add_subdirectory(lib/Catch2)
add_subdirectory(lib/SDL)

enable_testing()

if (MSVC)
	add_compile_options(/utf-8)
endif()

set(CRU_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(CRU_ASSETS_DIR ${PROJECT_SOURCE_DIR}/assets)

function(target_add_resources target res_dir)
	message("Add resources files in ${res_dir} to target ${target}.")

	file(GLOB_RECURSE RES_SOURCES "${CRU_ASSETS_DIR}/${res_dir}/*")
	target_sources(${target} PUBLIC ${RES_SOURCES})
	foreach (RES_FILE ${RES_SOURCES})
		file(RELATIVE_PATH RES_PATH ${CRU_ASSETS_DIR} ${RES_FILE})
		cmake_path(GET RES_PATH PARENT_PATH RES_PATH)
		set_property(SOURCE ${RES_FILE} PROPERTY MACOSX_PACKAGE_LOCATION "Resources/${RES_PATH}")
	endforeach(RES_FILE)
endfunction()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(demos)

if(EMSCRIPTEN)
	execute_process(COMMAND ${CMAKE_C_COMPILER} --cflags OUTPUT_VARIABLE CLANGD_FLAGS_TO_ADD)
	separate_arguments(CLANGD_FLAGS_TO_ADD UNIX_COMMAND "${CLANGD_FLAGS_TO_ADD}")
	list(JOIN CLANGD_FLAGS_TO_ADD ", " CLANGD_FLAGS_TO_ADD)
endif()

configure_file("${PROJECT_SOURCE_DIR}/scripts/clangd.in" "${CMAKE_BINARY_DIR}/clangd")

function(cru_process_dir dir)
	get_property(targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
	foreach(target ${targets})
		get_property(target_type TARGET "${target}" PROPERTY TYPE)
		if(NOT "${target_type}" MATCHES "^(EXECUTABLE|STATIC_LIBRARY|SHARED_LIBRARY|MODULE_LIBRARY|OBJECT_LIBRARY)$")
			continue()
		endif()

		if(WIN32)
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
