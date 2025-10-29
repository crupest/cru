add_library(CruPlatformGuiSdl
	UiApplication.cpp
	Window.cpp
)
target_link_libraries(CruPlatformGuiSdl PUBLIC
	CruPlatformGui CruPlatformGraphicsCairo
	SDL3::SDL3
)
