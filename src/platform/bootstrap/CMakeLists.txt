add_library(cru_platform_bootstrap SHARED
	Bootstrap.cpp
)

if(WIN32)
	target_link_libraries(cru_platform_bootstrap PUBLIC cru_win_gui)
elseif(APPLE)
	target_link_libraries(cru_platform_bootstrap PUBLIC cru_osx_gui)
endif()

target_compile_definitions(cru_platform_bootstrap PRIVATE CRU_PLATFORM_BOOTSTRAP_EXPORT_API)
