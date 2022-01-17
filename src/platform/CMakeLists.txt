add_library(cru_platform_base SHARED
	ForDllExport.cpp
	Color.cpp
)
target_link_libraries(cru_platform_base PUBLIC cru_base)
target_compile_definitions(cru_platform_base PRIVATE CRU_PLATFORM_EXPORT_API)

add_subdirectory(graphics)
add_subdirectory(gui)

if(WIN32)
	add_subdirectory(bootstrap)
elseif(APPLE)
	add_subdirectory(bootstrap)
endif()
