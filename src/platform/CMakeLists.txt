add_library(CruPlatformBase SHARED
	ForDllExport.cpp
	Color.cpp
	GraphicsBase.cpp
)
target_link_libraries(CruPlatformBase PUBLIC CruBase)
target_compile_definitions(CruPlatformBase PRIVATE CRU_PLATFORM_EXPORT_API)

add_subdirectory(graphics)
add_subdirectory(gui)

add_subdirectory(bootstrap)
