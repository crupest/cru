cmake_minimum_required(VERSION 3.21)

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

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(demos)
