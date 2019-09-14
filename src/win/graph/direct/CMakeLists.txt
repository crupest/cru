set(CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/win/graph/direct)

add_library(cru_win_graph_direct STATIC
	brush.cpp
	font.cpp
	geometry.cpp
	graph_factory.cpp
	painter.cpp
	text_layout.cpp
)
target_sources(cru_win_graph_direct PUBLIC
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/brush.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/com_resource.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/convert_util.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/direct_factory.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/exception.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/font.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/geometry.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/graph_factory.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/painter.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/platform_id.hpp
	${CRU_WIN_GRAPH_DIRECT_INCLUDE_DIR}/text_layout.hpp
)
target_link_libraries(cru_win_graph_direct PUBLIC D3D11 D2d1 DWrite cru_win_base cru_platform_graph)
