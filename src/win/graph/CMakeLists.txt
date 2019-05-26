set(CRU_WIN_GRAPH_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/win/graph)

add_library(cru_win_graph STATIC
	win_brush.cpp
	win_font.cpp
	win_geometry.cpp
	win_graph_factory.cpp
	win_painter.cpp
	win_text_layout.cpp
)
target_sources(cru_win_graph PUBLIC
	${CRU_WIN_GRAPH_INCLUDE_DIR}/win_brush.hpp
	${CRU_WIN_GRAPH_INCLUDE_DIR}/win_font.hpp
	${CRU_WIN_GRAPH_INCLUDE_DIR}/win_geometry.hpp
	${CRU_WIN_GRAPH_INCLUDE_DIR}/win_graph_factory.hpp
	${CRU_WIN_GRAPH_INCLUDE_DIR}/win_native_factory.hpp
	${CRU_WIN_GRAPH_INCLUDE_DIR}/win_painter.hpp
	${CRU_WIN_GRAPH_INCLUDE_DIR}/win_text_layout.hpp
)
target_link_libraries(cru_win_graph PUBLIC D3D11 D2d1 DWrite cru_win_base)
