set(CRU_PLATFORM_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform)
add_library(cru_platform_base STATIC
	Color.cpp
)
target_sources(cru_platform_base PUBLIC
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Check.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Color.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Exception.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/GraphBase.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/HeapDebug.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Matrix.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Resource.hpp
)
target_link_libraries(cru_platform_base PUBLIC cru_base)

add_subdirectory(graphics)
add_subdirectory(gui)
