set(CRU_PLATFORM_BASE_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform)
add_library(cru_platform_base SHARED
	ForDllExport.cpp
	Color.cpp
)
target_sources(cru_platform_base PUBLIC
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Base.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Check.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Color.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Exception.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/GraphicsBase.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/HeapDebug.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Matrix.hpp
	${CRU_PLATFORM_BASE_INCLUDE_DIR}/Resource.hpp
)
target_link_libraries(cru_platform_base PUBLIC cru_base)
target_compile_definitions(cru_platform_base PRIVATE CRU_PLATFORM_EXPORT_API)

add_subdirectory(graphics)
add_subdirectory(gui)

if(WIN32)
	add_subdirectory(bootstrap)
endif()
