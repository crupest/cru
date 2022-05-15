add_library(CruPlatformGui SHARED
	Keyboard.cpp
	Menu.cpp
	UiApplication.cpp
)
target_link_libraries(CruPlatformGui PUBLIC CruPlatformGraphics)
target_compile_definitions(CruPlatformGui PRIVATE CRU_PLATFORM_GUI_EXPORT_API)
