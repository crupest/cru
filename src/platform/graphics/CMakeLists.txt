add_library(CruPlatformGraphics
	ForDllExport.cpp
	Geometry.cpp
	Image.cpp
	NullPainter.cpp
	SvgGeometryBuilderMixin.cpp
)
target_compile_definitions(CruPlatformGraphics PRIVATE CRU_PLATFORM_GRAPHICS_EXPORT_API)
target_link_libraries(CruPlatformGraphics PUBLIC CruPlatformBase)

