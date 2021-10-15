set(CRU_PLATFORM_BOOTSTRAP_INCLUDE_DIR ${CRU_INCLUDE_DIR}/cru/platform/bootstrap)
add_library(cru_platform_bootstrap SHARED
	Bootstrap.cpp
)
target_sources(cru_platform_bootstrap PUBLIC
	${CRU_PLATFORM_BOOTSTRAP_INCLUDE_DIR}/Bootstrap.hpp
)

if(WIN32)
	target_link_libraries(cru_platform_bootstrap PUBLIC cru_win_gui)
elseif(APPLE)
	target_link_libraries(cru_platform_bootstrap PUBLIC cru_osx_gui)
endif()

target_compile_definitions(cru_platform_bootstrap PRIVATE CRU_PLATFORM_BOOTSTRAP_EXPORT_API)
