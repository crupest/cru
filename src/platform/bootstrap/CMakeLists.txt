set(CRU_PLATFORM_BOOTSTRAP_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform/bootstrap)
add_library(cru_platform_boostrap SHARED
	Bootstrap.cpp
)
target_sources(cru_platform_boostrap PUBLIC
	${CRU_PLATFORM_BOOTSTRAP_INCLUDE_DIR}/Bootstrap.hpp
)

if(WIN32)
	target_link_libraries(cru_platform_boostrap PUBLIC cru_win_gui)
endif()

target_compile_definitions(cru_platform_boostrap PRIVATE CRU_PLATFORM_BOOTSTRAP_EXPORT_API)
