find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

add_library(CruPlatformGuiSdl
	UiApplication.cpp
	Window.cpp
)
target_link_libraries(CruPlatformGuiSdl PUBLIC
	CruPlatformGui CruPlatformGraphicsCairo
	SDL2::SDL2
)
