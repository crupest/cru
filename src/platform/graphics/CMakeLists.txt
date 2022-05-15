add_library(CruPlatformGraphics SHARED
	ForDllExport.cpp
	Geometry.cpp
	Image.cpp
	NullPainter.cpp
)
target_compile_definitions(CruPlatformGraphics PRIVATE CRU_PLATFORM_GRAPHICS_EXPORT_API)
target_link_libraries(CruPlatformGraphics PUBLIC CruPlatformBase)

add_subdirectory(cairo)
