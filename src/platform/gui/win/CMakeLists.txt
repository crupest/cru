add_library(CruPlatformGuiWin
	Base.cpp
	Clipboard.cpp
	Cursor.cpp
	InputMethod.cpp
	Keyboard.cpp
	UiApplication.cpp
	Window.cpp
	WindowClass.cpp
)
target_link_libraries(CruPlatformGuiWin PUBLIC imm32)
target_link_libraries(CruPlatformGuiWin PUBLIC CruPlatformGraphicsDirect2d CruPlatformGui)
target_compile_definitions(CruPlatformGuiWin PRIVATE CRU_WIN_GUI_EXPORT_API)
