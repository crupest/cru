add_library(CruGraphics
	Base.cpp
	Color.cpp
	Geometry.cpp
	Image.cpp
	NullPainter.cpp
	SvgGeometryBuilderMixin.cpp
)
target_compile_definitions(CruGraphics PRIVATE CRU_GRAPHICS_EXPORT_API)
target_link_libraries(CruGraphics PUBLIC CruBase)

