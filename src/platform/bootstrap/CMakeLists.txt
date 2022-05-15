add_library(CruPlatformBootstrap SHARED
	Bootstrap.cpp
)

if(WIN32)
	target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGuiWin)
elseif(APPLE)
	target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGuiOsx)
else()
	target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGraphicsCairo)
endif()

target_compile_definitions(CruPlatformBootstrap PRIVATE CRU_PLATFORM_BOOTSTRAP_EXPORT_API)
