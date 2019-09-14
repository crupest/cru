set(CRU_PLATFORM_GRAPH_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform/graph)
add_library(cru_platform_graph INTERFACE)
target_sources(cru_platform_graph INTERFACE
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/brush.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/font.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/geometry.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/graph_factory.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/painter.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/text_layout.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/util/painter_util.hpp
)
target_link_libraries(cru_platform_graph INTERFACE cru_platform_base)
