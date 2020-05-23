set(CRU_PLATFORM_GRAPH_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform/graph)
add_library(cru_platform_graph INTERFACE)
target_sources(cru_platform_graph INTERFACE
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/Base.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/Brush.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/Font.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/Geometry.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/Factory.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/Resource.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/Painter.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/TextLayout.hpp
	${CRU_PLATFORM_GRAPH_INCLUDE_DIR}/util/Painter.hpp
)
target_link_libraries(cru_platform_graph INTERFACE cru_platform_base)
