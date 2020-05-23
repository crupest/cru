set(CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/win/graph/direct)

add_library(cru_win_graph_direct STATIC
	Brush.cpp
	Font.cpp
	Geometry.cpp
	Factory.cpp
	Painter.cpp
	Resource.cpp
	TextLayout.cpp
)
target_sources(cru_win_graph_direct PUBLIC
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/Brush.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/ComResource.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/ConvertUtil.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/Exception.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/Font.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/Geometry.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/Factory.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/Painter.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/Resource.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/TextLayout.hpp
)
target_link_libraries(cru_win_graph_direct PUBLIC D3D11 D2d1 DWrite)
target_link_libraries(cru_win_graph_direct PUBLIC cru_win_base cru_platform_graph)
