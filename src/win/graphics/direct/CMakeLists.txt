add_library(cru_win_graphics_direct STATIC
	Brush.cpp
	Font.cpp
	Geometry.cpp
	Factory.cpp
	Painter.cpp
	Resource.cpp
	TextLayout.cpp
	WindowPainter.cpp
	WindowRenderTarget.cpp
)
target_link_libraries(cru_win_graphics_direct PUBLIC D3D11 D2d1 DWrite)
target_link_libraries(cru_win_graphics_direct PUBLIC cru_win_base cru_platform_graphics)
