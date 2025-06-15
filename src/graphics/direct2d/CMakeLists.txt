add_library(CruPlatformGraphicsDirect2d
	Brush.cpp
	Font.cpp
	Geometry.cpp
	Image.cpp
	ImageFactory.cpp
	Factory.cpp
	Painter.cpp
	Resource.cpp
	TextLayout.cpp
	WindowPainter.cpp
	WindowRenderTarget.cpp
)
target_link_libraries(CruPlatformGraphicsDirect2d PUBLIC D3D11 D2d1 DWrite)
target_link_libraries(CruPlatformGraphicsDirect2d PUBLIC CruWinBase CruPlatformGraphics)
target_compile_definitions(CruPlatformGraphicsDirect2d PRIVATE CRU_WIN_GRAPHICS_DIRECT_EXPORT_API)
