add_library(CruPlatformBootstrap
	Bootstrap.cpp
)

if(WIN32)
	target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGuiWin)
elseif(APPLE)
	target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGuiOsx)
elseif(EMSCRIPTEN)
	target_link_libraries(CruPlatformBootstrap PUBLIC CruBase) # TODO: Remember to change this.
else()
	target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGraphicsCairo)
endif()

target_compile_definitions(CruPlatformBootstrap PRIVATE CRU_PLATFORM_BOOTSTRAP_EXPORT_API)
