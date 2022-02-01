add_library(cru_win_graphics_direct SHARED
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
target_link_libraries(cru_win_graphics_direct PUBLIC D3D11 D2d1 DWrite)
target_link_libraries(cru_win_graphics_direct PUBLIC cru_win_base cru_platform_graphics)
target_compile_definitions(cru_win_graphics_direct PRIVATE CRU_WIN_GRAPHICS_DIRECT_EXPORT_API)
