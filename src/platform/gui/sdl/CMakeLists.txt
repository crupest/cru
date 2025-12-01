add_library(CruPlatformGuiSdl
	Base.cpp
	Clipboard.cpp
	Cursor.cpp
	Input.cpp
	InputMethod.cpp
	UiApplication.cpp
	Window.cpp
)
target_link_libraries(CruPlatformGuiSdl PUBLIC
	CruPlatformGui CruPlatformGraphicsCairo
	SDL3::SDL3
)

if(UNIX AND NOT APPLE)
	target_sources(CruPlatformGuiSdl PRIVATE
		gl.c
		OpenGLRenderer.cpp
	)
endif()
