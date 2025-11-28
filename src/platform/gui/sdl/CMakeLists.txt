add_library(CruPlatformGuiSdl
	Base.cpp
	Cursor.cpp
	Input.cpp
	UiApplication.cpp
	Window.cpp
)
target_link_libraries(CruPlatformGuiSdl PUBLIC
	CruPlatformGui CruPlatformGraphicsCairo
	SDL3::SDL3
)
