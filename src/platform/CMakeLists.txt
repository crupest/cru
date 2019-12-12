set(CRU_PLATFORM_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform)
add_library(cru_platform_base INTERFACE)
target_sources(cru_platform_base INTERFACE
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/check.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/exception.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/graph_base.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/heap_debug.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/matrix.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/resource.hpp
)
target_link_libraries(cru_platform_base INTERFACE cru_base)

add_subdirectory(graph)
add_subdirectory(native)
