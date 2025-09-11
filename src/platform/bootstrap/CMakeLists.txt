add_library(CruPlatformGraphicsBootstrap
	GraphicsBootstrap.cpp
)

add_library(CruPlatformBootstrap
	Bootstrap.cpp
)

target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGraphicsBootstrap)

if(WIN32)
	target_link_libraries(CruPlatformGraphicsBootstrap PUBLIC CruPlatformGraphicsDirect)
	target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGuiWin)
elseif(APPLE)
	target_link_libraries(CruPlatformGraphicsBootstrap PUBLIC CruPlatformGraphicsQuartz)
	target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGuiOsx)
elseif(EMSCRIPTEN)
	target_link_libraries(CruPlatformGraphicsBootstrap PUBLIC CruBase)
	target_link_libraries(CruPlatformBootstrap PUBLIC CruBase) # TODO: Remember to change this.
else()
	target_link_libraries(CruPlatformGraphicsBootstrap PUBLIC CruPlatformGraphicsCairo)
	target_link_libraries(CruPlatformBootstrap PUBLIC CruPlatformGraphicsCairo)
endif()

target_compile_definitions(CruPlatformBootstrap PRIVATE CRU_PLATFORM_BOOTSTRAP_EXPORT_API)
