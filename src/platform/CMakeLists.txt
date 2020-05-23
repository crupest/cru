set(CRU_PLATFORM_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform)
add_library(cru_platform_base INTERFACE)
target_sources(cru_platform_base INTERFACE
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Check.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Exception.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/GraphBase.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/HeapDebug.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Matrix.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Resource.hpp
)
target_link_libraries(cru_platform_base INTERFACE cru_base)

add_subdirectory(graph)
add_subdirectory(native)
