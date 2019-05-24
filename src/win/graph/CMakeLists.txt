add_library(cru_win_graph STATIC
	win_brush.cpp
	win_font.cpp
	win_geometry.cpp
	win_graph_factory.cpp
	win_painter.cpp
	win_text_layout.cpp)
target_link_libraries(cru_win_graph PUBLIC D3D11 D2d1 DWrite cru_win_base)
