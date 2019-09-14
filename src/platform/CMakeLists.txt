set(CRU_PLATFORM_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform)
add_library(cru_platform_base INTERFACE)
target_sources(cru_platform_base INTERFACE
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/exception.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/graphic_base.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/heap_debug.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/matrix.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/native_resource.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/string_util.hpp
)
target_link_libraries(cru_platform_base INTERFACE cru_base)

add_subdirectory(graph)
add_subdirectory(native)
