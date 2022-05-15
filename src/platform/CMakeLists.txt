add_library(CruPlatformBase SHARED
	ForDllExport.cpp
	Color.cpp
	GraphicsBase.cpp
)
target_link_libraries(CruPlatformBase PUBLIC CruBase)
target_compile_definitions(CruPlatformBase PRIVATE CRU_PLATFORM_EXPORT_API)

add_subdirectory(graphics)
add_subdirectory(gui)

if (WIN32)
	add_subdirectory(win)
	add_subdirectory(graphics/direct2d)
	add_subdirectory(gui/win)
elseif (APPLE)
	add_subdirectory(osx)
	add_subdirectory(graphics/quartz)
	add_subdirectory(gui/osx)
endif()

add_subdirectory(bootstrap)
