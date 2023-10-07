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

function (patch_compile_commands input output_var)
	set(ENTRY_FILE ${CMAKE_BINARY_DIR}/compile_commands_entry)
	set(RESULT "[]")
	string(JSON COUNT LENGTH "${input}")
	message("Total commands need to patch: " ${COUNT})
	math(EXPR COUNT "${COUNT} - 1")
	foreach(INDEX RANGE ${COUNT})
		message("Patching: " ${INDEX} "/" ${COUNT})
		string(JSON ORIGINAL_COMMAND GET "${input}" ${INDEX} command)
		string(JSON WORKING_DIR GET "${input}" ${INDEX} directory)
		string(FIND "${ORIGINAL_COMMAND}" " " COMMAND_FIRST_SPACE)
		string(SUBSTRING "${ORIGINAL_COMMAND}" 0 ${COMMAND_FIRST_SPACE} COMMAND_COMPILER)
		string(SUBSTRING "${ORIGINAL_COMMAND}" ${COMMAND_FIRST_SPACE} -1 ORIGINAL_COMMAND_ARGS)
		string(CONCAT COMMAND_GENERATE "${COMMAND_COMPILER}" " -MJ ${ENTRY_FILE}" "${ORIGINAL_COMMAND_ARGS}")
		#message("Run: " "${COMMAND_GENERATE}")
		execute_process(COMMAND sh -c "${COMMAND_GENERATE}" WORKING_DIRECTORY "${WORKING_DIR}")
		file(READ ${ENTRY_FILE} ENTRY)
		string(JSON RESULT SET "${RESULT}" ${INDEX} "${ENTRY}")
	endforeach()
	set(${output_var} "${RESULT}" PARENT_SCOPE)
endfunction()

function (patch_compile_commands_file)
	file(READ ${CMAKE_BINARY_DIR}/compile_commands.json COMPILE_COMMANDS)
	patch_compile_commands("${COMPILE_COMMANDS}" COMPILE_COMMANDS_PATCHED)
	file(WRITE ${CMAKE_BINARY_DIR}/compile_commands-patched.json "${COMPILE_COMMANDS_PATCHED}")
endfunction()

