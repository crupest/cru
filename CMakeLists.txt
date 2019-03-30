cmake_minimum_required(VERSION 3.12) # use 3.12 because VS internal cmake is 3.12 

project(CruUI)


if(NOT CRU_CPU_ARCH)
    set(CRU_CPU_ARCH x64)
    message(STATUS "CPU arch not specified, use x64.")
endif()

set(CMAKE_CXX_STANDARD 17)

add_library(skia STATIC IMPORTED)
if(${CRU_SKIA_USE_CLANG})
    set_target_properties(skia PROPERTIES IMPORTED_LOCATION_DEBUG ${PROJECT_SOURCE_DIR}/skia/out/Debug_${CRU_CPU_ARCH}_clang/skia.lib
        IMPORTED_LOCATION_RELEASE ${PROJECT_SOURCE_DIR}/skia/out/Release_${CRU_CPU_ARCH}_clang/skia.lib)
else()
    set_target_properties(skia PROPERTIES IMPORTED_LOCATION_DEBUG ${PROJECT_SOURCE_DIR}/skia/out/Debug_${CRU_CPU_ARCH}/skia.lib
    IMPORTED_LOCATION_RELEASE ${PROJECT_SOURCE_DIR}/skia/out/Release_${CRU_CPU_ARCH}/skia.lib)
endif()

target_include_directories(skia INTERFACE skia/include)

add_subdirectory(src)
