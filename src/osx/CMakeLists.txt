add_library(cru_osx_base SHARED
	Convert.cpp
	Exception.cpp
	Resource.cpp
)

find_library(FOUNDATION Foundation REQUIRED)
find_library(CORE_FOUNDATION CoreFoundation REQUIRED)

target_link_libraries(cru_osx_base PUBLIC cru_platform_base ${FOUNDATION} ${CORE_FOUNDATION})

add_subdirectory(graphics)
add_subdirectory(gui)
