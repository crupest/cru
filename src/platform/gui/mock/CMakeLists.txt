add_library(CruPlatformGuiMock
	Base.cpp
	Clipboard.cpp
	Cursor.cpp
	InputMethod.cpp
	UiApplication.cpp
	Window.cpp
)
target_link_libraries(CruPlatformGuiMock PUBLIC CruPlatformGui)
target_compile_definitions(CruPlatformGuiMock PRIVATE CRU_PLATFORM_GUI_MOCK_EXPORT_API)
